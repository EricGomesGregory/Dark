// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dark/AbilitySystem/Abilities/DarkGameplayAbility.h"
#include "DarkGameplayAbility_Jump.generated.h"

/**
 * UDarkGameplayAbility_Jump
 *
 *	Gameplay ability used for character jumping.
 */
UCLASS(Abstract)
class DARK_API UDarkGameplayAbility_Jump : public UDarkGameplayAbility
{
	GENERATED_BODY()
	
public:
	UDarkGameplayAbility_Jump(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(BlueprintCallable, Category = "Dark|Ability")
	void CharacterJumpStart();

	UFUNCTION(BlueprintCallable, Category = "Dark|Ability")
	void CharacterJumpStop();
};
