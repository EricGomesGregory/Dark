// Fill out your copyright notice in the Description page of Project Settings.


#include "DarkGameplayAbility_Jump.h"

#include "Dark/Character/DarkCharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DarkGameplayAbility_Jump)


UDarkGameplayAbility_Jump::UDarkGameplayAbility_Jump(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UDarkGameplayAbility_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	const auto* DarkCharacter = Cast<ADarkCharacter>(ActorInfo->AvatarActor.Get());
	if (!DarkCharacter || !DarkCharacter->CanJump())
	{
		return false;
	}

	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void UDarkGameplayAbility_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// Stop jumping in case the ability blueprint doesn't call it.
	CharacterJumpStop();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UDarkGameplayAbility_Jump::CharacterJumpStart()
{
	if (ADarkCharacter* DarkCharacter = GetDarkCharacterFromActorInfo())
	{
		if (DarkCharacter->IsLocallyControlled() && !DarkCharacter->bPressedJump)
		{
			DarkCharacter->UnCrouch();
			DarkCharacter->Jump();
		}
	}
}

void UDarkGameplayAbility_Jump::CharacterJumpStop()
{
	if (ADarkCharacter* DarkCharacter = GetDarkCharacterFromActorInfo())
	{
		if (DarkCharacter->IsLocallyControlled() && DarkCharacter->bPressedJump)
		{
			DarkCharacter->StopJumping();
		}
	}
}
