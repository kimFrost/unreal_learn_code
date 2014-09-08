

#pragma once

#include "Pickup.h"
#include "BatteryPickup.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL_LEARN_CODE_API ABatteryPickup : public APickup
{
	GENERATED_UCLASS_BODY()

	/** Set the amount of power the battery gives to the player */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Power)
	float PowerLevel;

	/** Override the OnPickedUp function (Use Implementation because this is a BlueprintNativeEvent) */
	void OnPickedUp_Implementation() override;

	
};
