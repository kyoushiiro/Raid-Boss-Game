// Fill out your copyright notice in the Description page of Project Settings.


#include "RBProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ARBProjectile::ARBProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
}

// Called when the game starts or when spawned
void ARBProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

