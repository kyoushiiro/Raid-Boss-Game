// Fill out your copyright notice in the Description page of Project Settings.


#include "RBEngineSubsystem.h"
#include "AbilitySystemGlobals.h"

void URBEngineSubsystem::Initialize(FSubsystemCollectionBase& Collection) {
   Super::Initialize(Collection);

   UAbilitySystemGlobals::Get().InitGlobalData();
}
