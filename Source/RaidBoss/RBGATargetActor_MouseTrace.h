// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor_Trace.h"
#include "RBGATargetActor_MouseTrace.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class RAIDBOSS_API ARBGATargetActor_MouseTrace : public AGameplayAbilityTargetActor_Trace
{
	GENERATED_BODY()
	
public:
   ARBGATargetActor_MouseTrace();

   virtual void Tick(float deltaSeconds) override;

   void AimWithMouse(const AActor* InSourceActor, FCollisionQueryParams Params, const FVector& TraceStart, FVector& OutTraceEnd, bool bIgnorePitch=false);

protected:
	virtual FHitResult PerformTrace(AActor* InSourceActor) override;

   UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Abilities")
   UDecalComponent* ArrowDecal;
};
