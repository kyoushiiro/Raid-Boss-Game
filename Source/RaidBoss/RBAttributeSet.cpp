// Fill out your copyright notice in the Description page of Project Settings.


#include "RBAttributeSet.h"
#include "Net/UnrealNetwork.h"

void URBAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
   Super::GetLifetimeReplicatedProps(OutLifetimeProps);

   DOREPLIFETIME_CONDITION_NOTIFY(URBAttributeSet, Health, COND_None, REPNOTIFY_Always);
   DOREPLIFETIME_CONDITION_NOTIFY(URBAttributeSet, CharacterLevel, COND_None, REPNOTIFY_Always);
}

void URBAttributeSet::OnRep_CharacterLevel(const FGameplayAttributeData& OldLevel)
{
   GAMEPLAYATTRIBUTE_REPNOTIFY(URBAttributeSet, CharacterLevel, OldLevel);
}

void URBAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) {
   GAMEPLAYATTRIBUTE_REPNOTIFY(URBAttributeSet, Health, OldHealth);
}  

