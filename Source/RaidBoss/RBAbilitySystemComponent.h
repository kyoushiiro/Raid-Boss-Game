// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "RBAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class RAIDBOSS_API URBAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
   bool CharacterAbilitiesGiven;
   bool StartupEffectsApplied;
};
