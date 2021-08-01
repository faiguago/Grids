// Copyright Epic Games, Inc. All Rights Reserved.

#include "Illuvium_TestGameMode.h"
#include "Illuvium_TestCharacter.h"
#include "UObject/ConstructorHelpers.h"

AIlluvium_TestGameMode::AIlluvium_TestGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
