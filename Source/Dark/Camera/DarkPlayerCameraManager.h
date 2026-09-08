// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "DarkPlayerCameraManager.generated.h"

class UDarkUICameraManagerComponent;

#define DARK_CAMERA_DEFAULT_FOV			(80.0f)
#define DARK_CAMERA_DEFAULT_PITCH_MIN	(-89.0f)
#define DARK_CAMERA_DEFAULT_PITCH_MAX	(89.0f)


/**
 * 
 */
UCLASS(notplaceable)
class DARK_API ADarkPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
public:
	ADarkPlayerCameraManager(const FObjectInitializer& ObjectInitializer);

	UDarkUICameraManagerComponent* GetUICameraComponent() const;

protected:

	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;

	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;

private:
	/** The UI Camera Component, controls the camera when UI is doing something important that gameplay doesn't get priority over. */
	UPROPERTY(Transient)
	TObjectPtr<UDarkUICameraManagerComponent> UICamera;
};
