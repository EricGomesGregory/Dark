// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DarkPlayerController.generated.h"

class ADarkHUD;
class UDarkAbilitySystemComponent;
class ADarkPlayerState;


/**
 * ADarkPlayerController
 *
 *	The base player controller class used by this project.
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The base player controller class used by this project."))
class DARK_API ADarkPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ADarkPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Dark|PlayerController")
	ADarkPlayerState* GetDarkPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "Dark|PlayerController")
	UDarkAbilitySystemComponent* GetDarkAbilitySystemComponent() const;

	UFUNCTION(BlueprintCallable, Category = "Dark|PlayerController")
	ADarkHUD* GetDarkHUD() const;
	
	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~End of AActor interface

	//~AController interface
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void InitPlayerState() override;
	virtual void CleanupPlayerState() override;
	virtual void OnRep_PlayerState() override;
	//~End of AController interface

	//~APlayerController interface
	virtual void ReceivedPlayer() override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetPlayer(UPlayer* InPlayer) override;
	virtual void AddCheats(bool bForce) override;
	virtual void UpdateForceFeedback(IInputInterface* InputInterface, const int32 ControllerId) override;
	virtual void UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents) override;
	virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	//~End of APlayerController interface
	
protected:
	// Called when the player state is set or cleared
	virtual void OnPlayerStateChanged();

	//void OnSettingsChanged(UDarkSettingsShared* Settings);
	
private:
	void BroadcastOnPlayerStateChanged();
};
