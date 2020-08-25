// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "RBPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class RAIDBOSS_API ARBPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
   ARBPlayerState();

   class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

   class URBAttributeSet* GetAttributeSet() const;

   /**
     * Getters for attributes from RBAttributeSetBase
     */

   UFUNCTION(BlueprintCallable, Category = "RaidBoss|Attributes")
   float GetHealth() const;

   UFUNCTION(BlueprintCallable, Category = "RaidBoss|Attributes")
   int32 GetCharacterlevel() const;

protected:
   UPROPERTY()
   class URBAbilitySystemComponent* AbilitySystemComponent;

   UPROPERTY()
   class URBAttributeSet* AttributeSet;

   FGameplayTag DeadTag;

   FDelegateHandle HealthChangedDelegateHandle;
   FDelegateHandle CharacterLevelChangedDelegateHandle;

   virtual void BeginPlay() override;

   virtual void HealthChanged(const FOnAttributeChangeData& Data);
   virtual void CharacterLevelChanged(const FOnAttributeChangeData& Data);
};
