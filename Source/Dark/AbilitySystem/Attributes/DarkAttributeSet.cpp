// Fill out your copyright notice in the Description page of Project Settings.


#include "DarkAttributeSet.h"

#include "Dark/AbilitySystem/DarkAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DarkAttributeSet)


UDarkAttributeSet::UDarkAttributeSet()
{
}

UWorld* UDarkAttributeSet::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);

	return Outer->GetWorld();
}

UDarkAbilitySystemComponent* UDarkAttributeSet::GetDarkAbilitySystemComponent() const
{
	return Cast<UDarkAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}
