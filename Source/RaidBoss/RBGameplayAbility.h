// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RaidBoss.h"
#include "Abilities/GameplayAbility.h"
#include "RBGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class RAIDBOSS_API URBGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
   URBGameplayAbility();

    // Abilities with this set will automatically activate when input is pressed
   UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
   ERBAbilityInputID AbilityInputID = ERBAbilityInputID::None;

   // Value to associate an ability with a slot without automatically activating on input
   UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
   ERBAbilityInputID AbilityID = ERBAbilityInputID::None;

   // Tells ability to activate immediately on granted.
   UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
   bool ActivateAbilityOnGranted = false;

   // If ability should be activated on granted, activate them here.
   virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
};
