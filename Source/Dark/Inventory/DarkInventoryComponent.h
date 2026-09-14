// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "DarkInventoryComponent.generated.h"

struct FDarkInventoryList;
class UDarkInventoryItemInstance;
class UDarkInventoryItemDefinition;
class UDarkInventoryComponent;


/** 
 * FDarkInventoryChangeMessage
 * 
 *	A message when an item is added to the inventory
 */
USTRUCT(BlueprintType)
struct FDarkInventoryChangeMessage
{
	GENERATED_BODY()

	//@Eric TODO: Tag based names+owning actors for inventories instead of directly exposing the component? 
	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<UActorComponent> InventoryOwner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<UDarkInventoryItemInstance> Instance = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 NewCount = 0;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 Delta = 0;
};



/** 
 * FDarkInventoryEntry
 * 
 *	A single entry in an inventory 
 */
USTRUCT(BlueprintType)
struct FDarkInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FDarkInventoryEntry()
	{}

	FString GetDebugString() const;

private:
	friend FDarkInventoryList;
	friend UDarkInventoryComponent;

	UPROPERTY()
	TObjectPtr<UDarkInventoryItemInstance> Instance = nullptr;

	UPROPERTY()
	int32 StackCount = 0;

	UPROPERTY(NotReplicated)
	int32 LastObservedCount = INDEX_NONE;
};


/** 
 * FDarkInventoryList
 * 
 *	List of inventory items 
 */
USTRUCT(BlueprintType)
struct FDarkInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()

	FDarkInventoryList()
		: OwnerComponent(nullptr)
	{
	}

	FDarkInventoryList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

	TArray<UDarkInventoryItemInstance*> GetAllItems() const;

public:
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FDarkInventoryEntry, FDarkInventoryList>(Entries, DeltaParms, *this);
	}

	UDarkInventoryItemInstance* AddEntry(TSubclassOf<UDarkInventoryItemDefinition> ItemDefinition, int32 StackCount);
	void AddEntry(UDarkInventoryItemInstance* Instance);

	void RemoveEntry(UDarkInventoryItemInstance* Instance);

private:
	void BroadcastChangeMessage(FDarkInventoryEntry& Entry, int32 OldCount, int32 NewCount);

private:
	friend UDarkInventoryComponent;

private:
	// Replicated list of items
	UPROPERTY()
	TArray<FDarkInventoryEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FDarkInventoryList> : public TStructOpsTypeTraitsBase2<FDarkInventoryList>
{
	enum { WithNetDeltaSerializer = true };
};



/**
 * Manages an inventory
 */
UCLASS(ClassGroup=(Dark), BlueprintType, meta=(BlueprintSpawnableComponent))
class DARK_API UDarkInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDarkInventoryComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	bool CanAddItemDefinition(TSubclassOf<UDarkInventoryItemDefinition> ItemDefinition, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	UDarkInventoryItemInstance* AddItemDefinition(TSubclassOf<UDarkInventoryItemDefinition> ItemDefinition, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	void AddItemInstance(UDarkInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	void RemoveItemInstance(UDarkInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, Category=Inventory, BlueprintPure=false)
	TArray<UDarkInventoryItemInstance*> GetAllItems() const;

	UFUNCTION(BlueprintCallable, Category=Inventory, BlueprintPure)
	UDarkInventoryItemInstance* FindFirstItemStackByDefinition(TSubclassOf<UDarkInventoryItemDefinition> ItemDefinition) const;

	int32 GetTotalItemCountByDefinition(TSubclassOf<UDarkInventoryItemDefinition> ItemDefinition) const;
	bool ConsumeItemsByDefinition(TSubclassOf<UDarkInventoryItemDefinition> ItemDefinition, int32 NumToConsume);

	//~UObject interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	//~End of UObject interface

private:
	UPROPERTY(Replicated)
	FDarkInventoryList InventoryList;
};
