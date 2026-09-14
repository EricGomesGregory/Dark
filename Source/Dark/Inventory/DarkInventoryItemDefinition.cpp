// Fill out your copyright notice in the Description page of Project Settings.


#include "DarkInventoryItemDefinition.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(DarkInventoryItemDefinition)

UDarkInventoryItemDefinition::UDarkInventoryItemDefinition(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

const UDarkInventoryItemFragment* UDarkInventoryItemDefinition::FindFragmentByClass(TSubclassOf<UDarkInventoryItemFragment> FragmentClass) const
{
	if (FragmentClass != nullptr)
	{
		for (UDarkInventoryItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}

	return nullptr;
}

const TArray<UDarkInventoryItemFragment*> UDarkInventoryItemDefinition::FindFragmentsByClass(TSubclassOf<UDarkInventoryItemFragment> FragmentClass) const
{
	TArray<UDarkInventoryItemFragment*> Result;
	
	if (FragmentClass != nullptr)
	{
		for (UDarkInventoryItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				Result.AddUnique(Fragment);
			}
		}
	}
	return Result;
}
