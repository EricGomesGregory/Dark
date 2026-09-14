// Fill out your copyright notice in the Description page of Project Settings.


#include "DarkEquipmentGameplayAbility.h"

#include "DarkEquipmentInstance.h"
#include "Dark/Inventory/DarkInventoryItemInstance.h"
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(DarkEquipmentGameplayAbility)


UDarkEquipmentGameplayAbility::UDarkEquipmentGameplayAbility(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

UDarkEquipmentInstance* UDarkEquipmentGameplayAbility::GetAssociatedEquipment() const
{
	if (const FGameplayAbilitySpec* Spec = UGameplayAbility::GetCurrentAbilitySpec())
	{
		return Cast<UDarkEquipmentInstance>(Spec->SourceObject.Get());
	}
	return nullptr;
}

UDarkInventoryItemInstance* UDarkEquipmentGameplayAbility::GetAssociatedItem() const
{
	if (const UDarkEquipmentInstance* Equipment = GetAssociatedEquipment())
	{
		return Cast<UDarkInventoryItemInstance>(Equipment->GetInstigator());
	}
	return nullptr;
}

#if WITH_EDITOR
EDataValidationResult UDarkEquipmentGameplayAbility::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	if (InstancingPolicy == EGameplayAbilityInstancingPolicy::NonInstanced)
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
	{
		Context.AddError(NSLOCTEXT("Dark", "EquipmentAbilityMustBeInstanced", "Equipment ability must be instanced"));
		Result = EDataValidationResult::Invalid;
	}
	
	return Result;
}
#endif
