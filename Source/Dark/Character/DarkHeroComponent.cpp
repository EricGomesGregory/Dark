// Fill out your copyright notice in the Description page of Project Settings.


#include "DarkHeroComponent.h"

#include "DarkCharacter.h"
#include "Dark/DarkLogChannels.h"
#include "Dark/DarkGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "DarkPawnExtensionComponent.h"
#include "Dark/Player/DarkLocalPlayer.h"
#include "Dark/Player/DarkPlayerState.h"
#include "Dark/Input/DarkInputComponent.h"
#include "EnhancedInputSubsystemInterface.h"
#include "Dark/Player/DarkPlayerController.h"
#include "Dark/AbilitySystem/DarkAbilitySet.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Dark/AbilitySystem/DarkAbilitySystemComponent.h"
#include "Dark/Camera/DarkCameraComponent.h"
#include "Dark/Camera/DarkCameraMode.h"

#if WITH_EDITOR
#include "Misc/UObjectToken.h"
#endif	// WITH_EDITOR


#include UE_INLINE_GENERATED_CPP_BY_NAME(DarkHeroComponent)

namespace DarkHero
{
	static const float LookYawRate = 300.0f;
	static const float LookPitchRate = 165.0f;
};


const FName UDarkHeroComponent::NAME_BindInputsNow("BindInputsNow");
const FName UDarkHeroComponent::NAME_ActorFeatureName("Hero");

UDarkHeroComponent::UDarkHeroComponent(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	AbilityCameraMode = nullptr;
	bReadyToBindInputs = false;
}

void UDarkHeroComponent::SetAbilityCameraMode(TSubclassOf<UDarkCameraMode> CameraMode, const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	if (CameraMode)
	{
		AbilityCameraMode = CameraMode;
		AbilityCameraModeOwningSpecHandle = OwningSpecHandle;
	}
}

void UDarkHeroComponent::ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	if (AbilityCameraModeOwningSpecHandle == OwningSpecHandle)
	{
		AbilityCameraMode = nullptr;
		AbilityCameraModeOwningSpecHandle = FGameplayAbilitySpecHandle();
	}
}

void UDarkHeroComponent::AddAdditionalInputConfig(const UDarkInputConfig* InputConfig)
{
	TArray<uint32> BindHandles;

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	auto* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	if (const auto* PawnExtComp = UDarkPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		auto* DarkIC = Pawn->FindComponentByClass<UDarkInputComponent>();
		if (ensureMsgf(DarkIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UDarkInputComponent or a subclass of it.")))
		{
			DarkIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
		}
	}
}

void UDarkHeroComponent::RemoveAdditionalInputConfig(const UDarkInputConfig* InputConfig)
{
	//@Eric TODO: Implement me!
}

bool UDarkHeroComponent::IsReadyToBindInputs() const
{
	return bReadyToBindInputs;
}

bool UDarkHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();

	if (!CurrentState.IsValid() && DesiredState == DarkGameplayTags::InitState_Spawned)
	{
		// As long as we have a real pawn, let us transition
		if (Pawn)
		{
			return true;
		}
	}
	else if (CurrentState == DarkGameplayTags::InitState_Spawned && DesiredState == DarkGameplayTags::InitState_DataAvailable)
	{
		// The player state is required.
		if (!GetPlayerState<ADarkPlayerState>())
		{
			return false;
		}

		// If we're authority or autonomous, we need to wait for a controller with registered ownership of the player state.
		if (Pawn->GetLocalRole() != ROLE_SimulatedProxy)
		{
			AController* Controller = GetController<AController>();

			const bool bHasControllerPairedWithPS = (Controller != nullptr) && \
				(Controller->PlayerState != nullptr) && \
				(Controller->PlayerState->GetOwner() == Controller);

			if (!bHasControllerPairedWithPS)
			{
				return false;
			}
		}

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const bool bIsBot = Pawn->IsBotControlled();

		if (bIsLocallyControlled && !bIsBot)
		{
			ADarkPlayerController* DarkPC = GetController<ADarkPlayerController>();

			// The input component and local player is required when locally controlled.
			if (!Pawn->InputComponent || !DarkPC || !DarkPC->GetLocalPlayer())
			{
				return false;
			}
		}

		return true;
	}
	else if (CurrentState == DarkGameplayTags::InitState_DataAvailable && DesiredState == DarkGameplayTags::InitState_DataInitialized)
	{
		// Wait for player state and extension component
		auto* DarkPS = GetPlayerState<ADarkPlayerState>();

		return DarkPS && Manager->HasFeatureReachedInitState(Pawn, UDarkPawnExtensionComponent::NAME_ActorFeatureName, DarkGameplayTags::InitState_DataInitialized);
	}
	else if (CurrentState == DarkGameplayTags::InitState_DataInitialized && DesiredState == DarkGameplayTags::InitState_GameplayReady)
	{
		//@Eric TODO: add ability initialization checks? 
		return true;
	}

	return false;
}

void UDarkHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	if (CurrentState == DarkGameplayTags::InitState_DataAvailable && DesiredState == DarkGameplayTags::InitState_DataInitialized)
	{
		const APawn* Pawn = GetPawn<APawn>();
		ADarkPlayerState* DarkPS = GetPlayerState<ADarkPlayerState>();
		if (!ensure(Pawn && DarkPS))
		{
			return;
		}
		
		if (UDarkPawnExtensionComponent* PawnExtComp = UDarkPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			auto* DarkASC = DarkPS->GetDarkAbilitySystemComponent();
			// The player state holds the persistent data for this player (state that persists across deaths and multiple pawns).
			// The ability system component and attribute sets live on the player state.
			PawnExtComp->InitializeAbilitySystem(DarkASC, DarkPS);
			
			check(TagRelationshipMapping);
			DarkASC->SetTagRelationshipMapping(TagRelationshipMapping);
			
			for (const UDarkAbilitySet* AbilitySet : DefaultAbilitySets)
			{
				AbilitySet->GiveToAbilitySystem(DarkASC, nullptr);
			}
		}

		if (ADarkPlayerController* DarkPC = GetController<ADarkPlayerController>())
		{
			if (Pawn->InputComponent != nullptr)
			{
				InitializePlayerInput(Pawn->InputComponent);
			}
		}
		
		if (auto* CameraComponent = UDarkCameraComponent::FindCameraComponent(Pawn))
		{
			CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
		}
	}
}

void UDarkHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == UDarkPawnExtensionComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == DarkGameplayTags::InitState_DataInitialized)
		{
			// If the extension component says all other components are initialized, try to progress to next state
			CheckDefaultInitialization();
		}
	}
}

void UDarkHeroComponent::CheckDefaultInitialization()
{
	static const TArray<FGameplayTag> StateChain = {
		DarkGameplayTags::InitState_Spawned, 
		DarkGameplayTags::InitState_DataAvailable, 
		DarkGameplayTags::InitState_DataInitialized, 
		DarkGameplayTags::InitState_GameplayReady
	};

	// This will try to progress from spawned (which is only set in BeginPlay) through the data initialization stages until it gets to gameplay ready
	ContinueInitStateChain(StateChain);
}

void UDarkHeroComponent::OnRegister()
{
	Super::OnRegister();
	
	if (!GetPawn<APawn>())
	{
		UE_LOG(LogDark, Error, TEXT("[UDarkHeroComponent::OnRegister] This component has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint."));

#if WITH_EDITOR
		if (GIsEditor)
		{
			static const FText Message = NSLOCTEXT("DarkHeroComponent", "NotOnPawnError", "has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint. This will cause a crash if you PIE!");
			static const FName HeroMessageLogName = TEXT("DarkHeroComponent");
			
			FMessageLog(HeroMessageLogName).Error()
				->AddToken(FUObjectToken::Create(this, FText::FromString(GetNameSafe(this))))
				->AddToken(FTextToken::Create(Message));
				
			FMessageLog(HeroMessageLogName).Open();
		}
#endif
	}
	else
	{
		// Register with the init state system early, this will only work if this is a game world
		RegisterInitStateFeature();
	}
}

void UDarkHeroComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Listen for when the pawn extension component changes init state
	BindOnActorInitStateChanged(UDarkPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	// Notifies that we are done spawning, then try the rest of initialization
	ensure(TryToChangeInitState(DarkGameplayTags::InitState_Spawned));
	CheckDefaultInitialization();
}

void UDarkHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();
	
	Super::EndPlay(EndPlayReason);
}

void UDarkHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const auto* LP = Cast<UDarkLocalPlayer>(PC->GetLocalPlayer());
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();

	if (const UDarkPawnExtensionComponent* PawnExtComp = UDarkPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		for (const FInputMappingContextAndPriority& Mapping : DefaultInputMappings)
		{
			if (UInputMappingContext* IMC = Mapping.InputMapping.LoadSynchronous())
			{
				if (Mapping.bRegisterWithSettings)
				{
					if (UEnhancedInputUserSettings* Settings = Subsystem->GetUserSettings())
					{
						Settings->RegisterInputMappingContext(IMC);
					}
						
					FModifyContextOptions Options = {};
					Options.bIgnoreAllPressedKeysUntilRelease = false;
					// Actually add the config to the local player							
					Subsystem->AddMappingContext(IMC, Mapping.Priority, Options);
				}
			}
		}

		// The Dark Input Component has some additional functions to map Gameplay Tags to an Input Action.
		// If you want this functionality but still want to change your input component class, make it a subclass
		// of the UDarkInputComponent or modify this component accordingly.
		UDarkInputComponent* DarkIC = Cast<UDarkInputComponent>(PlayerInputComponent);
		if (ensureMsgf(DarkIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UDarkInputComponent or a subclass of it.")))
		{
			// Add the key mappings that may have been set by the player
			DarkIC->AddInputMappings(DefaultInputConfig, Subsystem);

			// This is where we actually bind and input action to a gameplay tag, which means that Gameplay Ability Blueprints will
			// be triggered directly by these input actions Triggered events. 
			TArray<uint32> BindHandles;
			DarkIC->BindAbilityActions(DefaultInputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);

			DarkIC->BindNativeAction(DefaultInputConfig, DarkGameplayTags::Input_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
			DarkIC->BindNativeAction(DefaultInputConfig, DarkGameplayTags::Input_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
			DarkIC->BindNativeAction(DefaultInputConfig, DarkGameplayTags::Input_Look_Stick, ETriggerEvent::Triggered, this, &ThisClass::Input_LookGamepad, /*bLogIfNotFound=*/ false);
			DarkIC->BindNativeAction(DefaultInputConfig, DarkGameplayTags::Input_Crouch, ETriggerEvent::Triggered, this, &ThisClass::Input_Crouch, /*bLogIfNotFound=*/ false);
		}
	}

	if (ensure(!bReadyToBindInputs))
	{
		bReadyToBindInputs = true;
	}
 
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PC), NAME_BindInputsNow);
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}

void UDarkHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const auto* PawnExtComp = UDarkPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			if (auto* DarkASC = PawnExtComp->GetDarkAbilitySystemComponent())
			{
				DarkASC->AbilityInputTagPressed(InputTag);
			}
		}	
	}
}

void UDarkHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const auto* PawnExtComp = UDarkPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			if (auto* DarkASC = PawnExtComp->GetDarkAbilitySystemComponent())
			{
				DarkASC->AbilityInputTagReleased(InputTag);
			}
		}
	}
}

void UDarkHeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	//@Eric TODO: Handle sprinting 
	
	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void UDarkHeroComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y);
	}
}

void UDarkHeroComponent::Input_LookGamepad(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();

	const UWorld* World = GetWorld();
	check(World);

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X * DarkHero::LookYawRate * World->GetDeltaSeconds());
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y * DarkHero::LookPitchRate * World->GetDeltaSeconds());
	}
}

void UDarkHeroComponent::Input_Crouch(const FInputActionValue& InputActionValue)
{
	if (auto* Character = GetPawn<ADarkCharacter>())
	{
		Character->ToggleCrouch();
	}
}

TSubclassOf<UDarkCameraMode> UDarkHeroComponent::DetermineCameraMode() const
{
	if (AbilityCameraMode)
	{
		return AbilityCameraMode;
	}

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return nullptr;
	}
	
	return DefaultCameraMode;
}

