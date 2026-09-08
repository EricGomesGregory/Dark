// Fill out your copyright notice in the Description page of Project Settings.


#include "DarkGameInstance.h"

#include "Components/GameFrameworkComponentManager.h"
#include "Dark/Player/DarkPlayerController.h"
#include "Dark/DarkGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DarkGameInstance)


class UGameFrameworkComponentManager;

UDarkGameInstance::UDarkGameInstance(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

ADarkPlayerController* UDarkGameInstance::GetPrimaryPlayerController() const
{
	return Cast<ADarkPlayerController>(Super::GetPrimaryPlayerController(false));
}

void UDarkGameInstance::Init()
{
	Super::Init();
	
	// Register our custom init states
	auto* ComponentManager = GetSubsystem<UGameFrameworkComponentManager>(this);

	if (ensure(ComponentManager))
	{
		ComponentManager->RegisterInitState(DarkGameplayTags::InitState_Spawned, false, FGameplayTag());
		ComponentManager->RegisterInitState(DarkGameplayTags::InitState_DataAvailable, false, DarkGameplayTags::InitState_Spawned);
		ComponentManager->RegisterInitState(DarkGameplayTags::InitState_DataInitialized, false, DarkGameplayTags::InitState_DataAvailable);
		ComponentManager->RegisterInitState(DarkGameplayTags::InitState_GameplayReady, false, DarkGameplayTags::InitState_DataInitialized);
	}
}

void UDarkGameInstance::Shutdown()
{
	Super::Shutdown();
}
