// Fill out your copyright notice in the Description page of Project Settings.


#include "DarkInputComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DarkInputComponent)


UDarkInputComponent::UDarkInputComponent(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UDarkInputComponent::AddInputMappings(const UDarkInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// Here you can handle any custom logic to add something from your input config if required
}

void UDarkInputComponent::RemoveInputMappings(const UDarkInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// Here you can handle any custom logic to remove input mappings that you may have added above
}

void UDarkInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
