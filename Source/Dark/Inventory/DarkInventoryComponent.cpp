// Fill out your copyright notice in the Description page of Project Settings.


#include "DarkInventoryComponent.h"

#include "DarkInventoryItemDefinition.h"
#include "DarkInventoryItemInstance.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DarkInventoryComponent)


FString FDarkInventoryEntry::GetDebugString() const
{
	TSubclassOf<UDarkInventoryItemDefinition> ItemDefinition;
	if (Instance != nullptr)
	{
		ItemDefinition = Instance->GetItemDefinition();
	}

	return FString::Printf(TEXT("%s (%d x %s)"), *GetNameSafe(Instance), StackCount, *GetNameSafe(ItemDefinition));
}



TArray<UDarkInventoryItemInstance*> FDarkInventoryList::GetAllItems() const
{
	TArray<UDarkInventoryItemInstance*> Results;
	Results.Reserve(Entries.Num());
	for (const FDarkInventoryEntry& Entry : Entries)
	{
		if (Entry.Instance != nullptr) //@Eric TODO: Would prefer to not deal with this here and hide it further?
		{
			Results.Add(Entry.Instance);
		}
	}
	return Results;
}

void FDarkInventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		FDarkInventoryEntry& Stack = Entries[Index];
		BroadcastChangeMessage(Stack, /*OldCount=*/ Stack.StackCount, /*NewCount=*/ 0);
		Stack.LastObservedCount = 0;
	}
}

void FDarkInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		FDarkInventoryEntry& Stack = Entries[Index];
		BroadcastChangeMessage(Stack, /*OldCount=*/ 0, /*NewCount=*/ Stack.StackCount);
		Stack.LastObservedCount = Stack.StackCount;
	}
}

void FDarkInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		FDarkInventoryEntry& Stack = Entries[Index];
		check(Stack.LastObservedCount != INDEX_NONE);
		BroadcastChangeMessage(Stack, /*OldCount=*/ Stack.LastObservedCount, /*NewCount=*/ Stack.StackCount);
		Stack.LastObservedCount = Stack.StackCount;
	}
}

UDarkInventoryItemInstance* FDarkInventoryList::AddEntry(TSubclassOf<UDarkInventoryItemDefinition> ItemDefinition, int32 StackCount)
{
	UDarkInventoryItemInstance* Result = nullptr;

	check(ItemDefinition != nullptr);
	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());


	FDarkInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	//@Eric TODO: Using the actor instead of component as the outer due to UE-127172
	NewEntry.Instance = NewObject<UDarkInventoryItemInstance>(OwningActor);
	NewEntry.Instance->SetItemDefinition(ItemDefinition);
	for (UDarkInventoryItemFragment* Fragment : GetDefault<UDarkInventoryItemDefinition>(ItemDefinition)->Fragments)
	{
		if (Fragment != nullptr)
		{
			Fragment->OnInstanceCreated(NewEntry.Instance);
		}
	}
	NewEntry.StackCount = StackCount;
	Result = NewEntry.Instance;

	//const UDarkInventoryItemDefinition* ItemCDO = GetDefault<UDarkInventoryItemDefinition>(ItemDef);
	MarkItemDirty(NewEntry);

	return Result;
}

void FDarkInventoryList::AddEntry(UDarkInventoryItemInstance* Instance)
{
	unimplemented();
}

void FDarkInventoryList::RemoveEntry(UDarkInventoryItemInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FDarkInventoryEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

void FDarkInventoryList::BroadcastChangeMessage(FDarkInventoryEntry& Entry, int32 OldCount, int32 NewCount)
{
	FDarkInventoryChangeMessage Message;
	Message.InventoryOwner = OwnerComponent;
	Message.Instance = Entry.Instance;
	Message.NewCount = NewCount;
	Message.Delta = NewCount - OldCount;

	//@Eric TODO: Uncomment after adding GameplayMessageSubsystem
	//UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	//MessageSystem.BroadcastMessage(DarkGameplayTags::Dark_Inventory_Message_StackChanged, Message);
}



UDarkInventoryComponent::UDarkInventoryComponent(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer), InventoryList(this)
{
	SetIsReplicatedByDefault(true);
}

void UDarkInventoryComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventoryList);
}

bool UDarkInventoryComponent::CanAddItemDefinition(TSubclassOf<UDarkInventoryItemDefinition> ItemDefinition, int32 StackCount)
{
	//@TODO: Add support for stack limit / uniqueness checks / etc...
	return true;
}

UDarkInventoryItemInstance* UDarkInventoryComponent::AddItemDefinition(TSubclassOf<UDarkInventoryItemDefinition> ItemDefinition, int32 StackCount)
{
	UDarkInventoryItemInstance* Result = nullptr;
	if (ItemDefinition != nullptr)
	{
		Result = InventoryList.AddEntry(ItemDefinition, StackCount);
	
		if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && Result)
		{
			AddReplicatedSubObject(Result);
		}
	}
	return Result;
}

void UDarkInventoryComponent::AddItemInstance(UDarkInventoryItemInstance* ItemInstance)
{
	InventoryList.AddEntry(ItemInstance);
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && ItemInstance)
	{
		AddReplicatedSubObject(ItemInstance);
	}
}

void UDarkInventoryComponent::RemoveItemInstance(UDarkInventoryItemInstance* ItemInstance)
{
	InventoryList.RemoveEntry(ItemInstance);

	if (ItemInstance && IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(ItemInstance);
	}
}

TArray<UDarkInventoryItemInstance*> UDarkInventoryComponent::GetAllItems() const
{
	return InventoryList.GetAllItems();
}

UDarkInventoryItemInstance* UDarkInventoryComponent::FindFirstItemStackByDefinition(TSubclassOf<UDarkInventoryItemDefinition> ItemDefinition) const
{
	for (const FDarkInventoryEntry& Entry : InventoryList.Entries)
	{
		UDarkInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDefinition() == ItemDefinition)
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

int32 UDarkInventoryComponent::GetTotalItemCountByDefinition(TSubclassOf<UDarkInventoryItemDefinition> ItemDefinition) const
{
	int32 TotalCount = 0;
	for (const FDarkInventoryEntry& Entry : InventoryList.Entries)
	{
		UDarkInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDefinition() == ItemDefinition)
			{
				++TotalCount;
			}
		}
	}

	return TotalCount;
}

bool UDarkInventoryComponent::ConsumeItemsByDefinition(TSubclassOf<UDarkInventoryItemDefinition> ItemDefinition, int32 NumToConsume)
{
	AActor* OwningActor = GetOwner();
	if (!OwningActor || !OwningActor->HasAuthority())
	{
		return false;
	}

	//@Eric TODO: N squared right now as there's no acceleration structure 
	int32 TotalConsumed = 0;
	while (TotalConsumed < NumToConsume)
	{
		if (auto* Instance = UDarkInventoryComponent::FindFirstItemStackByDefinition(ItemDefinition))
		{
			InventoryList.RemoveEntry(Instance);
			++TotalConsumed;
		}
		else
		{
			return false;
		}
	}

	return TotalConsumed == NumToConsume;
}

bool UDarkInventoryComponent::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FDarkInventoryEntry& Entry : InventoryList.Entries)
	{
		UDarkInventoryItemInstance* Instance = Entry.Instance;

		if (Instance && IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

void UDarkInventoryComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register existing UDarkInventoryItemInstance
	if (IsUsingRegisteredSubObjectList())
	{
		for (const FDarkInventoryEntry& Entry : InventoryList.Entries)
		{
			UDarkInventoryItemInstance* Instance = Entry.Instance;

			if (IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}
