// Fill out your copyright notice in the Description page of Project Settings.


#include "RBGameplayAbility.h"
#include "RBAbilitySystemComponent.h"

URBGameplayAbility::URBGameplayAbility() {
   InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

   // Default tags that block this ability from activating
   ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Dead")));
}

void URBGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
   Super::OnAvatarSet(ActorInfo, Spec);

   if (ActivateAbilityOnGranted) {
      bool ActivatedAbility = ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
   }
}
