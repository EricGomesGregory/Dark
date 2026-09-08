// Fill out your copyright notice in the Description page of Project Settings.


#include "DarkPlayerCameraManager.h"

#include "DarkUICameraManagerComponent.h"
#include "DarkCameraComponent.h"
#include "Engine/Canvas.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DarkPlayerCameraManager)


static FName UICameraComponentName(TEXT("UICamera"));

ADarkPlayerCameraManager::ADarkPlayerCameraManager(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	DefaultFOV   = DARK_CAMERA_DEFAULT_FOV;
	ViewPitchMin = DARK_CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = DARK_CAMERA_DEFAULT_PITCH_MAX;

	UICamera = CreateDefaultSubobject<UDarkUICameraManagerComponent>(UICameraComponentName);
}

UDarkUICameraManagerComponent* ADarkPlayerCameraManager::GetUICameraComponent() const
{
	return UICamera;
}

void ADarkPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	// If the UI Camera is looking at something, let it have priority.
	if (UICamera->NeedsToUpdateViewTarget())
	{
		Super::UpdateViewTarget(OutVT, DeltaTime);
		UICamera->UpdateViewTarget(OutVT, DeltaTime);
		return;
	}
	
	Super::UpdateViewTarget(OutVT, DeltaTime);
}

void ADarkPlayerCameraManager::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos)
{
	check(Canvas);

	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;

	DisplayDebugManager.SetFont(GEngine->GetSmallFont());
	DisplayDebugManager.SetDrawColor(FColor::Yellow);
	DisplayDebugManager.DrawString(FString::Printf(TEXT("DarkPlayerCameraManager: %s"), *GetNameSafe(this)));

	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);

	const APawn* Pawn = (PCOwner ? PCOwner->GetPawn() : nullptr);

	if (const auto* CameraComponent = UDarkCameraComponent::FindCameraComponent(Pawn))
	{
		CameraComponent->DrawDebug(Canvas);
	}
}
