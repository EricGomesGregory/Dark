// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "DarkPlayerState.generated.h"

class ADarkPlayerController;
class UDarkAbilitySystemComponent;


/** Defines the types of client connected */
UENUM()
enum class EDarkPlayerConnectionType : uint8
{
	// An active player
	Player = 0,

	// Spectator connected to a running game
	LiveSpectator,

	// Spectating a demo recording offline
	ReplaySpectator,

	// A deactivated player (disconnected)
	InactivePlayer
};

/**
 * 
 */
UCLASS(Config = Game)
class DARK_API ADarkPlayerState : public APlayerState,
public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	ADarkPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	UFUNCTION(BlueprintCallable, Category = "Dark|PlayerState")
	ADarkPlayerController* GetDarkPlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "Dark|PlayerState")
	UDarkAbilitySystemComponent* GetDarkAbilitySystemComponent() const { return AbilitySystemComponent; }
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	//~End of AActor interface

	//~APlayerState interface
	virtual void Reset() override;
	virtual void ClientInitialize(AController* C) override;
	virtual void CopyProperties(APlayerState* PlayerState) override;
	virtual void OnDeactivated() override;
	virtual void OnReactivated() override;
	//~End of APlayerState interface
	
	void SetPlayerConnectionType(EDarkPlayerConnectionType NewType);
	EDarkPlayerConnectionType GetPlayerConnectionType() const { return MyPlayerConnectionType; }
	
private:
	// The ability system component sub-object used by player characters.
	UPROPERTY(VisibleAnywhere, Category = "Dark|PlayerState")
	TObjectPtr<UDarkAbilitySystemComponent> AbilitySystemComponent;
	
	// Health attribute set used by this actor.
	//UPROPERTY()
	//TObjectPtr<const UDarkHealthSet> HealthSet;
	
	UPROPERTY(Replicated)
	EDarkPlayerConnectionType MyPlayerConnectionType;
};
