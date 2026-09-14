// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dark/AbilitySystem/Abilities/DarkGameplayAbility.h"
#include "DarkEquipmentGameplayAbility.generated.h"

class UDarkInventoryItemInstance;
class UDarkEquipmentInstance;


/**
 * UDarkEquipmentGameplayAbility
 *
 *	An ability granted by and associated with an equipment instance
 */
UCLASS()
class DARK_API UDarkEquipmentGameplayAbility : public UDarkGameplayAbility
{
	GENERATED_BODY()
	
public:
	UDarkEquipmentGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="Dark|Ability")
	UDarkEquipmentInstance* GetAssociatedEquipment() const;

	UFUNCTION(BlueprintCallable, Category = "Dark|Ability")
	UDarkInventoryItemInstance* GetAssociatedItem() const;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
};
