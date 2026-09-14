// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DarkInventoryItemDefinition.generated.h"

class UDarkInventoryItemInstance;


/**
 * UDarkInventoryItemFragment
 * 
 *	Represents a fragment of an item definition
 */ 
UCLASS(MinimalAPI, DefaultToInstanced, EditInlineNew, Abstract)
class UDarkInventoryItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UDarkInventoryItemInstance* Instance) const {}
};


/**
 * UDarkInventoryItemDefinition
 * 
 *	
 */
UCLASS(Blueprintable, Const, Abstract)
class DARK_API UDarkInventoryItemDefinition : public UObject
{
	GENERATED_BODY()
	
public:
	UDarkInventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display, Instanced)
	TArray<TObjectPtr<UDarkInventoryItemFragment>> Fragments;

public:
	const UDarkInventoryItemFragment* FindFragmentByClass(TSubclassOf<UDarkInventoryItemFragment> FragmentClass) const;
	
	const TArray<UDarkInventoryItemFragment*> FindFragmentsByClass(TSubclassOf<UDarkInventoryItemFragment> FragmentClass) const;
};
