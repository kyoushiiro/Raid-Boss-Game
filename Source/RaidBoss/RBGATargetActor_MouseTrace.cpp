// Fill out your copyright notice in the Description page of Project Settings.


#include "RBGATargetActor_MouseTrace.h"
#include "DrawDebugHelpers.h"
#include "Abilities/GameplayAbility.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/DecalComponent.h"

ARBGATargetActor_MouseTrace::ARBGATargetActor_MouseTrace() {
   RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));

   ArrowDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("ArrowDecalComp"));
   ArrowDecal->SetupAttachment(RootComponent);
}

void ARBGATargetActor_MouseTrace::AimWithMouse(const AActor* InSourceActor, FCollisionQueryParams Params, const FVector& TraceStart, FVector& OutTraceEnd, bool bIgnorePitch) 
{
   if (!OwningAbility) // Server and launching client only
   {
      return;
   }

   APlayerController* PC = OwningAbility->GetCurrentActorInfo()->PlayerController.Get();
   check(PC);

   FVector ViewStart;
   FRotator ViewRot;
   PC->GetPlayerViewPoint(ViewStart, ViewRot);

   FVector MouseWorldLocation;
   FVector ViewDir;
   PC->DeprojectMousePositionToWorld(MouseWorldLocation, ViewDir);

   //const FVector ViewDir = ViewRot.Vector();
   FVector ViewEnd = ViewStart + (ViewDir * MaxRange * 10);

   //ClipCameraRayToAbilityRange(ViewStart, ViewDir, TraceStart, MaxRange, ViewEnd);

   FHitResult HitResult;
   LineTraceWithFilter(HitResult, InSourceActor->GetWorld(), Filter, ViewStart, ViewEnd, TraceProfile.Name, Params);

   const bool bUseTraceResult = HitResult.bBlockingHit && (FVector::DistSquared(TraceStart, HitResult.Location) <= (MaxRange * MaxRange));

   FVector PlayerPosition = InSourceActor->GetActorLocation();
   FVector MouseDirection = HitResult.Location - PlayerPosition;
   MouseDirection.Normalize();

   FVector AdjustedEnd = (bUseTraceResult) ? HitResult.Location : PlayerPosition + MouseDirection * MaxRange;//ViewEnd;
   if (bUseTraceResult) {
      float PercentageRange = FVector::Dist(TraceStart, HitResult.Location)/MaxRange;
      ArrowDecal->SetWorldScale3D(FVector(1.0, PercentageRange, 1.0));

      FVector AdjustedLocation = ArrowDecal->GetRelativeLocation();
      AdjustedLocation.X = PercentageRange > 0.2f ? -MaxRange * 0.5 * PercentageRange : 0;
      ArrowDecal->SetRelativeLocation(AdjustedLocation);
   }
   else {
      ArrowDecal->SetWorldScale3D(FVector(1.0, 1.0, 1.0));
      ArrowDecal->SetRelativeLocation(FVector(-MaxRange * 0.5, 0.0, 0.0));
   }

   FVector AdjustedAimDir = (AdjustedEnd - TraceStart).GetSafeNormal();
   if (AdjustedAimDir.IsZero())
   {
      AdjustedAimDir = ViewDir;
   }

   if (!bTraceAffectsAimPitch && bUseTraceResult)
   {
      FVector OriginalAimDir = (ViewEnd - TraceStart).GetSafeNormal();

      if (!OriginalAimDir.IsZero())
      {
         // Convert to angles and use original pitch
         const FRotator OriginalAimRot = OriginalAimDir.Rotation();

         FRotator AdjustedAimRot = AdjustedAimDir.Rotation();
         AdjustedAimRot.Pitch = OriginalAimRot.Pitch;

         AdjustedAimDir = AdjustedAimRot.Vector();
      }
   }

   OutTraceEnd = TraceStart + (AdjustedAimDir * MaxRange);
}

void ARBGATargetActor_MouseTrace::Tick(float deltaSeconds) {
	// very temp - do a mostly hardcoded trace from the source actor
   if (SourceActor)
   {
      FHitResult HitResult = PerformTrace(SourceActor);
      FVector EndPoint = HitResult.Component.IsValid() ? HitResult.ImpactPoint : HitResult.TraceEnd;

      #if ENABLE_DRAW_DEBUG
      if (bDebug)
      {
         DrawDebugLine(GetWorld(), SourceActor->GetActorLocation(), EndPoint, FColor::Green, false);
         DrawDebugSphere(GetWorld(), EndPoint, 16, 10, FColor::Green, false);
      }
      #endif // ENABLE_DRAW_DEBUG

      APlayerController* PC = OwningAbility->GetCurrentActorInfo()->PlayerController.Get();
      check(PC);

      FVector ActorLoc = SourceActor->GetActorLocation();
      ActorLoc.Z = EndPoint.Z;
      FVector Temp = EndPoint - ActorLoc;
      FRotator Direction = Temp.Rotation();
      //SetActorLocationAndRotation(EndPoint, SourceActor->GetActorRotation());
      SetActorLocationAndRotation(EndPoint, Direction);
   }
}

FHitResult ARBGATargetActor_MouseTrace::PerformTrace(AActor* InSourceActor)
{
   bool bTraceComplex = false;
   TArray<AActor*> ActorsToIgnore;

   ActorsToIgnore.Add(InSourceActor);

   FCollisionQueryParams Params(SCENE_QUERY_STAT(ARBGATargetActor_MouseTrace), bTraceComplex);
   Params.bReturnPhysicalMaterial = true;
   Params.AddIgnoredActors(ActorsToIgnore);

   FVector TraceStart = StartLocation.GetTargetingTransform().GetLocation();// InSourceActor->GetActorLocation();
   FVector TraceEnd;
   AimWithMouse(InSourceActor, Params, TraceStart, TraceEnd);		//Effective on server and launching client only

   // ------------------------------------------------------

   FHitResult ReturnHitResult;
   LineTraceWithFilter(ReturnHitResult, InSourceActor->GetWorld(), Filter, TraceStart, TraceEnd, TraceProfile.Name, Params);
   //Default to end of trace line if we don't hit anything.
   if (!ReturnHitResult.bBlockingHit)
   {
      ReturnHitResult.Location = TraceEnd;
   }
   if (AGameplayAbilityWorldReticle* LocalReticleActor = ReticleActor.Get())
   {
      const bool bHitActor = (ReturnHitResult.bBlockingHit && (ReturnHitResult.Actor != NULL));
      const FVector ReticleLocation = (bHitActor && LocalReticleActor->bSnapToTargetedActor) ? ReturnHitResult.Actor->GetActorLocation() : ReturnHitResult.Location;

      LocalReticleActor->SetActorLocation(ReticleLocation);
      LocalReticleActor->SetIsTargetAnActor(bHitActor);
   }

   #if ENABLE_DRAW_DEBUG
   if (bDebug)
   {
      DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Green);
      DrawDebugSphere(GetWorld(), TraceEnd, 100.0f, 16, FColor::Green);
   }
   #endif // ENABLE_DRAW_DEBUG
   return ReturnHitResult;
}
