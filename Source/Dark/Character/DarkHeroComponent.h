// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameplayTagContainer.h"
#include "Components/PawnComponent.h"
#include "Dark/Input/DarkInputTypes.h"
#include "Components/GameFrameworkComponentDelegates.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "DarkHeroComponent.generated.h"

class UDarkAbilityTagRelationshipMapping;
class UGameFrameworkComponentManager;
class UDarkInputConfig;
class UDarkAbilitySet;


/**
 * UDarkHeroComponent
 * 
 * 
 */
UCLASS(ClassGroup=(Dark), Blueprintable, meta=(BlueprintSpawnableComponent))
class DARK_API UDarkHeroComponent : public UPawnComponent, 
public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	UDarkHeroComponent(const FObjectInitializer& ObjectInitializer);

	/** Returns the hero component if one exists on the specified actor. */
	UFUNCTION(BlueprintPure, Category = "Dark|Hero")
	static UDarkHeroComponent* FindHeroComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UDarkHeroComponent>() : nullptr); }

	/** Adds mode-specific input config */
	void AddAdditionalInputConfig(const UDarkInputConfig* InputConfig);

	/** Removes a mode-specific input config if it has been added */
	void RemoveAdditionalInputConfig(const UDarkInputConfig* InputConfig);

	/** True if this is controlled by a real player and has progressed far enough in initialization where additional input bindings can be added */
	bool IsReadyToBindInputs() const;
	
	/** The name of the extension event sent via UGameFrameworkComponentManager when ability inputs are ready to bind */
	static const FName NAME_BindInputsNow;

	/** The name of this component-implemented feature */
	static const FName NAME_ActorFeatureName;

	//~ Begin IGameFrameworkInitStateInterface interface
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	//~ End IGameFrameworkInitStateInterface interface

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);
	void Input_LookGamepad(const FInputActionValue& InputActionValue);
	void Input_Crouch(const FInputActionValue& InputActionValue);
	
protected:
	UPROPERTY(EditAnywhere)
	TArray<FInputMappingContextAndPriority> DefaultInputMappings;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UDarkInputConfig> DefaultInputConfig;
	
	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<const UDarkAbilitySet>> DefaultAbilitySets;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UDarkAbilityTagRelationshipMapping> TagRelationshipMapping;
	
	/** True when player input bindings have been applied, will never be true for non - players */
	bool bReadyToBindInputs;
};
