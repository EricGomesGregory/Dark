// Fill out your copyright notice in the Description page of Project Settings.


#include "DarkUICameraManagerComponent.h"

#include "DarkPlayerCameraManager.h"
#include "GameFramework/HUD.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DarkUICameraManagerComponent)


UDarkUICameraManagerComponent* UDarkUICameraManagerComponent::GetComponent(APlayerController* PC)
{
	if (PC != nullptr)
	{
		if (auto* PCCamera = Cast<ADarkPlayerCameraManager>(PC->PlayerCameraManager))
		{
			return PCCamera->GetUICameraComponent();
		}
	}

	return nullptr;
}

UDarkUICameraManagerComponent::UDarkUICameraManagerComponent()
{
	bWantsInitializeComponent = true;

	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		// Register "show debug" hook.
		if (!IsRunningDedicatedServer())
		{
			AHUD::OnShowDebugInfo.AddUObject(this, &ThisClass::OnShowDebugInfo);
		}
	}
}

void UDarkUICameraManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UDarkUICameraManagerComponent::SetViewTarget(AActor* InViewTarget, FViewTargetTransitionParams TransitionParams)
{
	TGuardValue<bool> UpdatingViewTargetGuard(bUpdatingViewTarget, true);

	ViewTarget = InViewTarget;
	CastChecked<ADarkPlayerCameraManager>(GetOwner())->SetViewTarget(ViewTarget, TransitionParams);
}

bool UDarkUICameraManagerComponent::NeedsToUpdateViewTarget() const
{
	return false;
}

void UDarkUICameraManagerComponent::UpdateViewTarget(struct FTViewTarget& OutVT, float DeltaTime)
{
}

void UDarkUICameraManagerComponent::OnShowDebugInfo(AHUD* HUD, UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& YL, float& YPos)
{
}
