// Fill out your copyright notice in the Description page of Project Settings.


#include "RBPlayerState.h"
#include "RBAttributeSet.h"
#include "RaidBossCharacter.h"
#include "RBAbilitySystemComponent.h"

ARBPlayerState::ARBPlayerState() {
   AbilitySystemComponent = CreateDefaultSubobject<URBAbilitySystemComponent>(TEXT("AbilitySystemComp"));
   AbilitySystemComponent->SetIsReplicated(true);
   AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

   AttributeSet = CreateDefaultSubobject<URBAttributeSet>(TEXT("AttributeSet"));

   NetUpdateFrequency = 100.0f;

   DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
}

UAbilitySystemComponent* ARBPlayerState::GetAbilitySystemComponent() const
{
   return AbilitySystemComponent;
}

URBAttributeSet* ARBPlayerState::GetAttributeSet() const
{
   return AttributeSet;
}

float ARBPlayerState::GetHealth() const
{
   return AttributeSet->GetHealth();
}

int32 ARBPlayerState::GetCharacterlevel() const
{
   return static_cast<int32>(AttributeSet->GetCharacterLevel());
}

void ARBPlayerState::BeginPlay()
{
   Super::BeginPlay();

   if (AbilitySystemComponent) {
      HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &ARBPlayerState::HealthChanged);
      CharacterLevelChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetCharacterLevelAttribute()).AddUObject(this, &ARBPlayerState::CharacterLevelChanged);
   }
}

void ARBPlayerState::HealthChanged(const FOnAttributeChangeData& Data)
{
   float Health = Data.NewValue;

   UE_LOG(LogTemp, Warning, TEXT("HEALTH CHANGED"));
   ARaidBossCharacter* Hero = Cast<ARaidBossCharacter>(GetPawn());
   if (Hero) {
      UE_LOG(LogTemp, Warning, TEXT("HEALTH ON HERO IS NOW %f"), Health);
   }
}

void ARBPlayerState::CharacterLevelChanged(const FOnAttributeChangeData& Data)
{
   float Level = Data.NewValue;

   UE_LOG(LogTemp, Warning, TEXT("LEVEL CHANGED"));
   ARaidBossCharacter* Hero = Cast<ARaidBossCharacter>(GetPawn());
   if (Hero) {
      UE_LOG(LogTemp, Warning, TEXT("LEVEL ON HERO CHANGED"));
   }
}
