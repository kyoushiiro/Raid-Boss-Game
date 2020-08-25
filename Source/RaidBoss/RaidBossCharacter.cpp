// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "RaidBossCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "Engine/World.h"

#include "RBAttributeSet.h"
#include "RBAbilitySystemComponent.h"
#include "RBGameplayAbility.h"
#include "RBPlayerState.h"
#include "RaidBossPlayerController.h"

#include "Boss.h"

ARaidBossCharacter::ARaidBossCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 1600.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 900.f;
	CameraBoom->SetRelativeRotation(FRotator(-55.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a decal in the world to show the cursor's location
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
}

void ARaidBossCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	if (CursorToWorld != nullptr)
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			FHitResult TraceHitResult;
			PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();
			CursorToWorld->SetWorldLocation(TraceHitResult.Location);
			CursorToWorld->SetWorldRotation(CursorR);

			PC->GetHitResultUnderCursor(ECC_GameTraceChannel2, true, TraceHitResult);
			if (TraceHitResult.bBlockingHit) {
				ABoss* BossActor = Cast<ABoss>(TraceHitResult.GetActor());
				if (BossActor) {
					BossActor->SetSelected(true);
					Selected = BossActor;
				}
			}
			else {
            if (Selected != nullptr) {
               Selected->SetSelected(false);
               Selected = nullptr;
            }
			}
		}
	}
}

void ARaidBossCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	BindASCInput();
}

void ARaidBossCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ARBPlayerState* PS = GetPlayerState<ARBPlayerState>();
	if (PS) {
		// Set ASC on server, clients do this in OnRep_PlayerState
		AbilitySystemComponent = Cast<URBAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		// AI won't have PCs, so can init again here to be sure. No harm initing twice for hero PCs.
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);

		AttributeSet = PS->GetAttributeSet();

		InitializeAttributes();
		AddStartupEffects();
		AddCharacterAbilities();

		ARaidBossPlayerController* PC = Cast<ARaidBossPlayerController>(GetController());
		if (PC) {
		//	PC->CreateHUD();
		}

		// Respawn specific things that won't affect first possession.

		// Forcibly set DeadTag count to 0
		AbilitySystemComponent->SetTagMapCount(DeadTag, 0);

		// Set Health/Mana to max, only necessary for respawn
	}
}

UAbilitySystemComponent* ARaidBossCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

int32 ARaidBossCharacter::GetCharacterLevel() const
{
	if (AttributeSet.IsValid()) {
		return static_cast<int32>(AttributeSet->GetCharacterLevel());
	}

	return 0;
}

float ARaidBossCharacter::GetHeath() const
{
	if (AttributeSet.IsValid()) {
		return AttributeSet->GetHealth();
	}

	return 0.0f;
}

void ARaidBossCharacter::AddCharacterAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || AbilitySystemComponent->CharacterAbilitiesGiven) {
		return;
	}

	for (TSubclassOf<URBGameplayAbility>& StartupAbility : CharacterAbilities) {
		AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(StartupAbility, 
				GetAbilityLevel(StartupAbility.GetDefaultObject()->AbilityID),
				static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this)
		);
	}

	AbilitySystemComponent->CharacterAbilitiesGiven = true;
}

void ARaidBossCharacter::InitializeAttributes()
{
	if (!AbilitySystemComponent.IsValid()) {
		return;
	}
	
	if (!DefaultAttributes) {
		UE_LOG(LogTemp, Error, TEXT("%s() Missing DefaultAttributes for %s."), *FString(__FUNCTION__), *GetName());
	}

	// Can run on server and client
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, GetCharacterLevel(), EffectContext);
	if (NewHandle.IsValid()) {
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
	}
}

void ARaidBossCharacter::AddStartupEffects()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || AbilitySystemComponent->StartupEffectsApplied) {
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects) {
		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, GetCharacterLevel(), EffectContext);
		if (NewHandle.IsValid()) {
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
		}
	}

	AbilitySystemComponent->StartupEffectsApplied = true;
}

void ARaidBossCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	ARBPlayerState* PS = GetPlayerState<ARBPlayerState>();
	if (PS) {
		// Set ASC for clients, server does this in PossesedBy
		AbilitySystemComponent = Cast<URBAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		// Init ASC ActorInfo for clients, server inits its ASC when it possesses a new Actor
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);

		BindASCInput();

		AttributeSet = PS->GetAttributeSet();

		// For handling players disconnectin/rejoining in the future, we will need to change this so that
		// possession doesn't necessarily respawn attributes. For now, possession = spawn/respawn.
		InitializeAttributes();

		ARaidBossPlayerController* PC = Cast<ARaidBossPlayerController>(GetController());
		if (PC) {
			//PC->CreateHUD();
		}

		// Respawn specific things that won't affect first possession.

		// Forcibly set DeadTag count to 0
		AbilitySystemComponent->SetTagMapCount(DeadTag, 0);

		// Set Health/Mana to max - this is only necessary for respawns
	}
}

void ARaidBossCharacter::BindASCInput()
{
	if (!ASCInputBound && AbilitySystemComponent.IsValid() && IsValid(InputComponent)) {
		AbilitySystemComponent->BindAbilityActivationToInputComponent(
			InputComponent, 
			FGameplayAbilityInputBinds(FString("ConfirmTarget"), FString("CancelTarget"), FString("ERBAbilityInputID"),
				static_cast<int32>(ERBAbilityInputID::Confirm), static_cast<int32>(ERBAbilityInputID::Cancel))
		);

		ASCInputBound = true;
	}
}

void ARaidBossCharacter::SetInProgressAbility(URBGameplayAbility* Ability)
{
	InProgressAbility = Ability;
}

URBGameplayAbility* ARaidBossCharacter::GetInProgressAbility()
{
	return InProgressAbility;
}

int32 ARaidBossCharacter::GetAbilityLevel(ERBAbilityInputID AbilityID) const {
	return 1;
}
