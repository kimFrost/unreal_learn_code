// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "unreal_learn_code.h"
#include "unreal_learn_codeGameMode.h"
#include "unreal_learn_codeCharacter.h"

Aunreal_learn_codeGameMode::Aunreal_learn_codeGameMode(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/MyCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
