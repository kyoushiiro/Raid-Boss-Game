// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRaidBoss, Log, All);

UENUM(BlueprintType)
enum class ERBAbilityInputID : uint8 {
   None           UMETA(DisplayName = "None"),
   Confirm        UMETA(DisplayName = "Confirm"),
   Cancel         UMETA(DisplayName = "Cancel"),
   Ability1       UMETA(DisplayName = "Ability1"),
   Ability2       UMETA(DisplayName = "Ability2")
};
