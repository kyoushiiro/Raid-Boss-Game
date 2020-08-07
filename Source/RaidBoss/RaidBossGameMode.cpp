// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "RaidBossGameMode.h"
#include "RaidBossPlayerController.h"
#include "RaidBossCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARaidBossGameMode::ARaidBossGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ARaidBossPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}