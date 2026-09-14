// Fill out your copyright notice in the Description page of Project Settings.


#include "DarkInventoryItemInstance.h"

#include "Net/UnrealNetwork.h"
#include "DarkInventoryItemDefinition.h"
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DarkInventoryItemInstance)


UDarkInventoryItemInstance::UDarkInventoryItemInstance(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UDarkInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, StatTags);
	DOREPLIFETIME(ThisClass, ItemDefinition);
}

void UDarkInventoryItemInstance::AddStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.AddStack(Tag, StackCount);
}

void UDarkInventoryItemInstance::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.RemoveStack(Tag, StackCount);
}

int32 UDarkInventoryItemInstance::GetStatTagStackCount(FGameplayTag Tag) const
{
	return StatTags.GetStackCount(Tag);
}

bool UDarkInventoryItemInstance::HasStatTag(FGameplayTag Tag) const
{
	return StatTags.ContainsTag(Tag);
}

const UDarkInventoryItemFragment* UDarkInventoryItemInstance::FindFragmentByClass(TSubclassOf<UDarkInventoryItemFragment> FragmentClass) const
{
	if ((ItemDefinition != nullptr) && (FragmentClass != nullptr))
	{
		return GetDefault<UDarkInventoryItemDefinition>(ItemDefinition)->FindFragmentByClass(FragmentClass);
	}

	return nullptr;
}

TArray<UDarkInventoryItemFragment*> UDarkInventoryItemInstance::FindFragmentsByClass(TSubclassOf<UDarkInventoryItemFragment> FragmentClass) const
{
	if ((ItemDefinition != nullptr) && (FragmentClass != nullptr))
	{
		return GetDefault<UDarkInventoryItemDefinition>(ItemDefinition)->FindFragmentsByClass(FragmentClass);
	}

	return TArray<UDarkInventoryItemFragment*>();
}

void UDarkInventoryItemInstance::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags)
{
	using namespace UE::Net;

	// Build descriptors and allocate PropertyReplicationFragments for this object
	FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
}

void UDarkInventoryItemInstance::SetItemDefinition(TSubclassOf<UDarkInventoryItemDefinition> InDefinition)
{
	ItemDefinition = InDefinition;
}
