// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "RBAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
   GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
   GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
   GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
   GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName) 

/**
 * 
 */
UCLASS()
class RAIDBOSS_API URBAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:

   virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
   
   // Current character level.
   UPROPERTY(BlueprintReadOnly, Category = "CharacterLevel", ReplicatedUsing = OnRep_CharacterLevel)
   FGameplayAttributeData CharacterLevel;
   ATTRIBUTE_ACCESSORS(URBAttributeSet, CharacterLevel)

   // Current Health, clamped at MaxHealth.
   UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
   FGameplayAttributeData Health;
   ATTRIBUTE_ACCESSORS(URBAttributeSet, Health)

protected:
   /**
    * These OnRep functions exist to make sure that the ability system internal
    * representations are synchronized properly
    **/
   UFUNCTION()
   virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);
   
   UFUNCTION()
   virtual void OnRep_CharacterLevel(const FGameplayAttributeData& OldLevel);
};
