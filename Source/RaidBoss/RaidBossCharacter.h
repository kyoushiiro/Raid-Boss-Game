// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RaidBoss.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "RaidBossCharacter.generated.h"

class URBAbilitySystemComponent;
class URBAttributeSet;
class URBGameplayAbility;
class UGameplayEffect;

UCLASS(Blueprintable)
class ARaidBossCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ARaidBossCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CursorToWorld;

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Only called on Server, calls before server's acknowledge possession
	virtual void PossessedBy(AController* NewController) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/* Switch on AbilityID to return individual ability levels */
	virtual int32 GetAbilityLevel(ERBAbilityInputID AbilityID) const;


	/*
	 * Getters for attributes from RBAttributeSet.
	*/
	UFUNCTION(BlueprintCallable, Category = "RaidBoss|Attributes")
	int32 GetCharacterLevel() const;

	UFUNCTION(BlueprintCallable, Category = "RaidBoss|Attributes")
	float GetHeath() const;

protected:
	TWeakObjectPtr<URBAbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<URBAttributeSet> AttributeSet;

	// Default abilities to grant the character
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "RaidBoss|Abilities")
	TArray<TSubclassOf<URBGameplayAbility>> CharacterAbilities;
	
	// GE that defines the default attributes of the character
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "RaidBoss|Abilities")
	TSubclassOf<UGameplayEffect> DefaultAttributes;

	// GEs that are applied once at startup (i.e. heath regen, passives, etc)
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "RaidBoss|Abilities")
	TArray<TSubclassOf<UGameplayEffect>> StartupEffects;

	// Give abilities on server, AbilitySpecs will be replicated to the client.
	void AddCharacterAbilities();

	// Do on server, but also on client, since the attributes should be the same anyways,
	// so that we don't have to wait.
	void InitializeAttributes();

	void AddStartupEffects();

	// Client only
	virtual void OnRep_PlayerState() override;

	// Called from both SetupPlayerInputComponent and OnRep_PlayerState because of
	// potential race condition
	void BindASCInput();
	bool ASCInputBound = false;

	FGameplayTag DeadTag;

	UFUNCTION(BlueprintCallable, Category = "RaidBoss")
	void SetInProgressAbility(URBGameplayAbility* Ability);

	UFUNCTION(BlueprintCallable, Category = "RaidBoss")
	URBGameplayAbility* GetInProgressAbility();

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "RaidBoss")
	URBGameplayAbility* InProgressAbility;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "RaidBoss")
	class ABoss* Selected;

};

