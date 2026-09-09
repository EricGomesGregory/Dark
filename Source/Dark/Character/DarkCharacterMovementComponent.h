// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DarkCharacterMovementComponent.generated.h"


/**
 * FDarkCharacterGroundInfo
 *
 *	Information about the ground under the character.  It only gets updated as needed.
 */
USTRUCT(BlueprintType)
struct FDarkCharacterGroundInfo
{
	GENERATED_BODY()

	FDarkCharacterGroundInfo()
		: LastUpdateFrame(0)
		, GroundDistance(0.0f)
	{}

	uint64 LastUpdateFrame;

	UPROPERTY(BlueprintReadOnly)
	FHitResult GroundHitResult;

	UPROPERTY(BlueprintReadOnly)
	float GroundDistance;
};


/**
 * UDarkCharacterMovementComponent
 *
 *	The base character movement component class used by this project.
 */
UCLASS(Config = Game)
class DARK_API UDarkCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UDarkCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

	virtual void SimulateMovement(float DeltaTime) override;

	virtual bool CanAttemptJump() const override;

	// Returns the current ground info.  Calling this will update the ground info if it's out of date.
	UFUNCTION(BlueprintCallable, Category = "Dark|CharacterMovement")
	const FDarkCharacterGroundInfo& GetGroundInfo();

	void SetReplicatedAcceleration(const FVector& InAcceleration);

	//~UMovementComponent interface
	virtual FRotator GetDeltaRotation(float DeltaTime) const override;
	virtual float GetMaxSpeed() const override;
	//~End of UMovementComponent interface

protected:
	virtual void InitializeComponent() override;

protected:
	// Cached ground info for the character.  Do not access this directly!  It's only updated when accessed via GetGroundInfo().
	FDarkCharacterGroundInfo CachedGroundInfo;

	UPROPERTY(Transient)
	bool bHasReplicatedAcceleration = false;
};
