// Fill out your copyright notice in the Description page of Project Settings.


#include "DarkAbilitySet.h"

#include "DarkAbilitySystemComponent.h"
#include "Dark/DarkLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DarkAbilitySet)


void FDarkAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FDarkAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

void FDarkAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* Set)
{
	GrantedAttributeSets.Add(Set);
}

void FDarkAbilitySet_GrantedHandles::TakeFromAbilitySystem(UDarkAbilitySystemComponent* DarkASC)
{
	check(DarkASC);

	if (!DarkASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			DarkASC->ClearAbility(Handle);
		}
	}

	for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			DarkASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	for (UAttributeSet* Set : GrantedAttributeSets)
	{
		DarkASC->RemoveSpawnedAttribute(Set);
	}

	AbilitySpecHandles.Reset();
	GameplayEffectHandles.Reset();
	GrantedAttributeSets.Reset();
}



UDarkAbilitySet::UDarkAbilitySet(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UDarkAbilitySet::GiveToAbilitySystem(UDarkAbilitySystemComponent* DarkASC, FDarkAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
	check(DarkASC);

if (!DarkASC->IsOwnerActorAuthoritative())
{
	// Must be authoritative to give or take ability sets.
	return;
}

// Grant the attribute sets.
for (int32 SetIndex = 0; SetIndex < GrantedAttributes.Num(); ++SetIndex)
{
	const FDarkAbilitySet_AttributeSet& SetToGrant = GrantedAttributes[SetIndex];

	if (!IsValid(SetToGrant.AttributeSet))
	{
		UE_LOG(LogDarkAbilitySystem, Error, TEXT("GrantedAttributes[%d] on ability set [%s] is not valid"), SetIndex, *GetNameSafe(this));
		continue;
	}

	UAttributeSet* NewSet = NewObject<UAttributeSet>(DarkASC->GetOwner(), SetToGrant.AttributeSet);
	DarkASC->AddAttributeSetSubobject(NewSet);

	if (OutGrantedHandles)
	{
		OutGrantedHandles->AddAttributeSet(NewSet);
	}
}

// Grant the gameplay abilities.
for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
{
	const FDarkAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

	if (!IsValid(AbilityToGrant.Ability))
	{
		UE_LOG(LogDarkAbilitySystem, Error, TEXT("GrantedGameplayAbilities[%d] on ability set [%s] is not valid."), AbilityIndex, *GetNameSafe(this));
		continue;
	}

	UDarkGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UDarkGameplayAbility>();

	FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
	AbilitySpec.SourceObject = SourceObject;
	AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityToGrant.InputTag);

	const FGameplayAbilitySpecHandle AbilitySpecHandle = DarkASC->GiveAbility(AbilitySpec);

	if (OutGrantedHandles)
	{
		OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
	}
}

// Grant the gameplay effects.
for (int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); ++EffectIndex)
{
	const FDarkAbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

	if (!IsValid(EffectToGrant.GameplayEffect))
	{
		UE_LOG(LogDarkAbilitySystem, Error, TEXT("GrantedGameplayEffects[%d] on ability set [%s] is not valid"), EffectIndex, *GetNameSafe(this));
		continue;
	}

	const UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
	const FActiveGameplayEffectHandle GameplayEffectHandle = DarkASC->ApplyGameplayEffectToSelf(GameplayEffect, EffectToGrant.EffectLevel, DarkASC->MakeEffectContext());

	if (OutGrantedHandles)
	{
		OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
	}
}
}
