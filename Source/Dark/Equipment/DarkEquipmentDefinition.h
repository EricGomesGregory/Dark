// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DarkEquipmentDefinition.generated.h"

class UDarkEquipmentInstance;
class UDarkAbilitySet;


/**
 * FDarkEquipmentActorToSpawn
 * 
 * 
 */
USTRUCT()
struct FDarkEquipmentActorToSpawn
{
	GENERATED_BODY()

	FDarkEquipmentActorToSpawn()
	{}

	UPROPERTY(EditAnywhere, Category=Equipment)
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category=Equipment)
	FName AttachSocket;

	UPROPERTY(EditAnywhere, Category=Equipment)
	FTransform AttachTransform;
};


/**
 * UDarkEquipmentDefinition
 *
 *	Definition of a piece of equipment that can be applied to a pawn
 */
UCLASS(Blueprintable, Const, Abstract, BlueprintType)
class DARK_API UDarkEquipmentDefinition : public UObject
{
	GENERATED_BODY()
	
public:
	UDarkEquipmentDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Class to spawn
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TSubclassOf<UDarkEquipmentInstance> InstanceType;

	// Gameplay ability sets to grant when this is equipped
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TArray<TObjectPtr<const UDarkAbilitySet>> AbilitySetsToGrant;

	// Actors to spawn on the pawn when this is equipped
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TArray<FDarkEquipmentActorToSpawn> ActorsToSpawn;
};
