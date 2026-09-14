// Fill out your copyright notice in the Description page of Project Settings.


#include "DarkEquipmentComponent.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "Dark/DarkLogChannels.h"
#include "AbilitySystemGlobals.h"
#include "DarkEquipmentInstance.h"
#include "DarkEquipmentDefinition.h"
#include "Dark/AbilitySystem/DarkAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DarkEquipmentComponent)


// ======================================== //
//  FDarkAppliedEquipmentEntry              //
// ======================================== //

FString FDarkAppliedEquipmentEntry::GetDebugString() const
{
	return FString::Printf(TEXT("%s of %s"), *GetNameSafe(Instance), *GetNameSafe(EquipmentDefinition.Get()));
}


// ======================================== //
//  FDarkEquipmentList                      //
// ======================================== //

void FDarkEquipmentList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		const FDarkAppliedEquipmentEntry& Entry = Entries[Index];
		if (Entry.Instance != nullptr)
		{
			Entry.Instance->OnUnequipped();
		}
	}
}

void FDarkEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		const FDarkAppliedEquipmentEntry& Entry = Entries[Index];
		if (Entry.Instance != nullptr)
		{
			Entry.Instance->OnEquipped();
		}
	}
}

void FDarkEquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	//@Eric TODO: Implement this 
}

UDarkEquipmentInstance* FDarkEquipmentList::AddEntry(TSubclassOf<UDarkEquipmentDefinition> EquipmentDefinition)
{
	UDarkEquipmentInstance* Result = nullptr;

	check(EquipmentDefinition != nullptr);
	check(OwnerComponent);
	
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	const UDarkEquipmentDefinition* EquipmentCDO = GetDefault<UDarkEquipmentDefinition>(EquipmentDefinition);

	TSubclassOf<UDarkEquipmentInstance> InstanceType = EquipmentCDO->InstanceType;
	if (InstanceType == nullptr)
	{
		InstanceType = UDarkEquipmentInstance::StaticClass();
	}

	FDarkAppliedEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.EquipmentDefinition = EquipmentDefinition;
	//@Eric TODO: Using the actor instead of component as the outer due to UE-127172 
	NewEntry.Instance = NewObject<UDarkEquipmentInstance>(OwningActor, InstanceType);
	Result = NewEntry.Instance;

	if (UDarkAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		for (const TObjectPtr<const UDarkAbilitySet>& AbilitySet : EquipmentCDO->AbilitySetsToGrant)
		{
			AbilitySet->GiveToAbilitySystem(ASC, /*inout*/ &NewEntry.GrantedHandles, Result);
		}
	}
	else
	{
		UE_LOG(LogDarkEquipment, Warning, TEXT("No Equipment Definition Found"));
	}

	Result->SpawnEquipmentActors(EquipmentCDO->ActorsToSpawn);


	MarkItemDirty(NewEntry);

	return Result;
}

void FDarkEquipmentList::RemoveEntry(UDarkEquipmentInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FDarkAppliedEquipmentEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			if (UDarkAbilitySystemComponent* ASC = GetAbilitySystemComponent())
			{
				Entry.GrantedHandles.TakeFromAbilitySystem(ASC);
			}

			Instance->DestroyEquipmentActors();
		

			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

UDarkAbilitySystemComponent* FDarkEquipmentList::GetAbilitySystemComponent() const
{
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	return Cast<UDarkAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor));
}


// ======================================== //
//  UDarkEquipmentComponent                 //
// ======================================== //

UDarkEquipmentComponent::UDarkEquipmentComponent(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer), EquipmentList(this)
{
	SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;
}

void UDarkEquipmentComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquipmentList);
}

UDarkEquipmentInstance* UDarkEquipmentComponent::EquipItem(TSubclassOf<UDarkEquipmentDefinition> EquipmentDefinition)
{
	UDarkEquipmentInstance* Result = nullptr;
	if (EquipmentDefinition != nullptr)
	{
		Result = EquipmentList.AddEntry(EquipmentDefinition);
		if (Result != nullptr)
		{
			Result->OnEquipped();

			if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
			{
				AddReplicatedSubObject(Result);
			}
		}
	}
	return Result;
}

void UDarkEquipmentComponent::UnequipItem(UDarkEquipmentInstance* ItemInstance)
{
	if (ItemInstance != nullptr)
	{
		if (IsUsingRegisteredSubObjectList())
		{
			RemoveReplicatedSubObject(ItemInstance);
		}

		ItemInstance->OnUnequipped();
		EquipmentList.RemoveEntry(ItemInstance);
	}
}

bool UDarkEquipmentComponent::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FDarkAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		UDarkEquipmentInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

void UDarkEquipmentComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UDarkEquipmentComponent::UninitializeComponent()
{
	TArray<UDarkEquipmentInstance*> AllEquipmentInstances;

	// gathering all instances before removal to avoid side effects affecting the equipment list iterator	
	for (const FDarkAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		AllEquipmentInstances.Add(Entry.Instance);
	}

	for (UDarkEquipmentInstance* EquipInstance : AllEquipmentInstances)
	{
		UnequipItem(EquipInstance);
	}
	
	Super::UninitializeComponent();
}

void UDarkEquipmentComponent::ReadyForReplication()
{
	Super::ReadyForReplication();
	
	// Register existing DarkEquipmentInstances
	if (IsUsingRegisteredSubObjectList())
	{
		for (const FDarkAppliedEquipmentEntry& Entry : EquipmentList.Entries)
		{
			UDarkEquipmentInstance* Instance = Entry.Instance;

			if (IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

UDarkEquipmentInstance* UDarkEquipmentComponent::GetFirstInstanceOfType(TSubclassOf<UDarkEquipmentInstance> InstanceType)
{
	for (FDarkAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UDarkEquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceType))
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

TArray<UDarkEquipmentInstance*> UDarkEquipmentComponent::GetEquipmentInstancesOfType(TSubclassOf<UDarkEquipmentInstance> InstanceType) const
{
	TArray<UDarkEquipmentInstance*> Results;
	for (const FDarkAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UDarkEquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceType))
			{
				Results.Add(Instance);
			}
		}
	}
	return Results;
}
