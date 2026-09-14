// Fill out your copyright notice in the Description page of Project Settings.


#include "DarkItemFragment_SetStats.h"

#include "Dark/Inventory/DarkInventoryItemInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DarkItemFragment_SetStats)


void UDarkItemFragment_SetStats::OnInstanceCreated(UDarkInventoryItemInstance* Instance) const
{
	for (const auto& KVP : InitialItemStats)
	{
		Instance->AddStatTagStack(KVP.Key, KVP.Value);
	}
}

int32 UDarkItemFragment_SetStats::GetItemStatByTag(FGameplayTag Tag) const
{
	if (const int32* StatPtr = InitialItemStats.Find(Tag))
	{
		return *StatPtr;
	}

	return 0;
}
