// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "unreal_learn_code.h"
#include "unreal_learn_codeCharacter.h"
#include "BatteryPickup.h"

//////////////////////////////////////////////////////////////////////////
// Aunreal_learn_codeCharacter

Aunreal_learn_codeCharacter::Aunreal_learn_codeCharacter(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{


	// Set a base power level for the character
	PowerLevel = 2000.f;
	// Set the dependence of speed on the power level
	SpeedFactor = 0.75f;
	BaseSpeed = 10.0f;

	// Create out battery collection volume
	CollectionSphere = PCIP.CreateDefaultSubobject<USphereComponent>(this, TEXT("CollectionSphere"));
	CollectionSphere->AttachTo(RootComponent);
	CollectionSphere->SetSphereRadius(200.f);




	// Set size for collision capsule
	CapsuleComponent->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	CharacterMovement->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	CharacterMovement->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	CharacterMovement->JumpZVelocity = 600.f;
	CharacterMovement->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = PCIP.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUseControllerViewRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FollowCamera"));
	FollowCamera->AttachTo(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUseControllerViewRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void Aunreal_learn_codeCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up gameplay key bindings
	check(InputComponent);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	InputComponent->BindAction("CollectPickups", IE_Pressed, this, &Aunreal_learn_codeCharacter::CollectBatteries);

	InputComponent->BindAxis("MoveForward", this, &Aunreal_learn_codeCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &Aunreal_learn_codeCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &Aunreal_learn_codeCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &Aunreal_learn_codeCharacter::LookUpAtRate);

	// handle touch devices
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &Aunreal_learn_codeCharacter::TouchStarted);
}


void Aunreal_learn_codeCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	// jump, but only on the first touch
	if (FingerIndex == ETouchIndex::Touch1)
	{
		Jump();
	}
}

void Aunreal_learn_codeCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void Aunreal_learn_codeCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void Aunreal_learn_codeCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void Aunreal_learn_codeCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
void Aunreal_learn_codeCharacter::CollectBatteries()
{
	float BatteryPower = 0.f;

	// Get all overlapping actors and store them in a CollectedActors array
	TArray<AActor*> CollectedActors;
	CollectionSphere->GetOverlappingActors(CollectedActors);

	// For Each actors collected
	for (int32 iCollected = 0; iCollected < CollectedActors.Num(); ++iCollected)
	{
		// Cast the collected actor to ABatteryPickup
		ABatteryPickup* const TestBattery = Cast<ABatteryPickup>(CollectedActors[iCollected]);

		// If the cast is successful, and the battery is valid and active
		if (TestBattery && !TestBattery->IsPendingKill() && TestBattery->bIsActive)
		{
			// Store its battery power for adding to the character's power
			BatteryPower = BatteryPower + TestBattery->PowerLevel;
			// Deactivate the battery
			TestBattery->bIsActive = false;
			// Call the battery's OnPickedUp function
			TestBattery->OnPickedUp();
			
		}

	}

	if (BatteryPower > 0.f)
	{
		// Call the Blueprinted implementation of PowerUp with the total battery power as input
		PowerUp(BatteryPower);
	}

}
void Aunreal_learn_codeCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	CharacterMovement->MaxWalkSpeed = SpeedFactor * PowerLevel + BaseSpeed;
}