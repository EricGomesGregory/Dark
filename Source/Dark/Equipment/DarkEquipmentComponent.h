// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Dark/AbilitySystem/DarkAbilitySet.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "DarkEquipmentComponent.generated.h"

class UDarkEquipmentDefinition;
class UDarkAbilitySystemComponent;
class UDarkEquipmentComponent;
class UDarkEquipmentInstance;
struct FDarkEquipmentList;


/** 
 * FDarkAppliedEquipmentEntry
 * 
 *	A single piece of applied equipment 
 */
USTRUCT(BlueprintType)
struct FDarkAppliedEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FDarkAppliedEquipmentEntry()
	{}

	FString GetDebugString() const;

private:
	friend FDarkEquipmentList;
	friend UDarkEquipmentComponent;

	// The equipment class that got equipped
	UPROPERTY()
	TSubclassOf<UDarkEquipmentDefinition> EquipmentDefinition;

	UPROPERTY()
	TObjectPtr<UDarkEquipmentInstance> Instance = nullptr;

	// Authority-only list of granted handles
	UPROPERTY(NotReplicated)
	FDarkAbilitySet_GrantedHandles GrantedHandles;
};

/** List of applied equipment */
USTRUCT(BlueprintType)
struct FDarkEquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()

	FDarkEquipmentList()
		: OwnerComponent(nullptr)
	{
	}

	FDarkEquipmentList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

public:
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FDarkAppliedEquipmentEntry, FDarkEquipmentList>(Entries, DeltaParms, *this);
	}

	UDarkEquipmentInstance* AddEntry(TSubclassOf<UDarkEquipmentDefinition> EquipmentDefinition);
	void RemoveEntry(UDarkEquipmentInstance* Instance);

private:
	UDarkAbilitySystemComponent* GetAbilitySystemComponent() const;

	friend UDarkEquipmentComponent;

private:
	// Replicated list of equipment entries
	UPROPERTY()
	TArray<FDarkAppliedEquipmentEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FDarkEquipmentList> : public TStructOpsTypeTraitsBase2<FDarkEquipmentList>
{
	enum { WithNetDeltaSerializer = true };
};


/**
 * UDarkEquipmentComponent
 * 
 *	
 */
UCLASS(ClassGroup=(Dark), BlueprintType, Const, meta=(BlueprintSpawnableComponent))
class DARK_API UDarkEquipmentComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UDarkEquipmentComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	UDarkEquipmentInstance* EquipItem(TSubclassOf<UDarkEquipmentDefinition> EquipmentDefinition);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void UnequipItem(UDarkEquipmentInstance* ItemInstance);

	//~UObject interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	//~End of UObject interface

	//~UActorComponent interface
	//virtual void EndPlay() override;
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	virtual void ReadyForReplication() override;
	//~End of UActorComponent interface

	/** Returns the first equipped instance of a given type, or nullptr if none are found */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UDarkEquipmentInstance* GetFirstInstanceOfType(TSubclassOf<UDarkEquipmentInstance> InstanceType);

	/** Returns all equipped instances of a given type, or an empty array if none are found */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UDarkEquipmentInstance*> GetEquipmentInstancesOfType(TSubclassOf<UDarkEquipmentInstance> InstanceType) const;

	template <typename T>
	T* GetFirstInstanceOfType()
	{
		return const_cast<T*>(GetFirstInstanceOfType(T::StaticClass()));
	}

private:
	UPROPERTY(Replicated)
	FDarkEquipmentList EquipmentList;
};
