// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "RBEngineSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class RAIDBOSS_API URBEngineSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()
	
public:
   virtual void Initialize(FSubsystemCollectionBase& Collection) override;
};
