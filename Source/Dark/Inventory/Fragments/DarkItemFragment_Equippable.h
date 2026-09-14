// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dark/Inventory/DarkInventoryItemDefinition.h"
#include "DarkItemFragment_Equippable.generated.h"

class UDarkEquipmentDefinition;


/**
 * UDarkItemFragment_Equippable
 * 
 *	
 */
UCLASS(meta=(DisplayName="Equippable"))
class DARK_API UDarkItemFragment_Equippable : public UDarkInventoryItemFragment
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category=Dark)
	TSubclassOf<UDarkEquipmentDefinition> EquipmentDefinition;
};
