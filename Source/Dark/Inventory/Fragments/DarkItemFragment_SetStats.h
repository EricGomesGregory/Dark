// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Dark/Inventory/DarkInventoryItemDefinition.h"
#include "DarkItemFragment_SetStats.generated.h"

/**
 * 
 */
UCLASS(meta=(DisplayName="Set Stats"))
class DARK_API UDarkItemFragment_SetStats : public UDarkInventoryItemFragment
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TMap<FGameplayTag, int32> InitialItemStats;

public:
	virtual void OnInstanceCreated(UDarkInventoryItemInstance* Instance) const override;

	int32 GetItemStatByTag(FGameplayTag Tag) const;
};
