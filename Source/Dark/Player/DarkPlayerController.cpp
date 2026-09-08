// Fill out your copyright notice in the Description page of Project Settings.


#include "DarkPlayerController.h"

#include "Dark/AbilitySystem/DarkAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "DarkPlayerState.h"
#include "Dark/UI/DarkHUD.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DarkPlayerController)


ADarkPlayerController::ADarkPlayerController(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

ADarkPlayerState* ADarkPlayerController::GetDarkPlayerState() const
{
	return CastChecked<ADarkPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UDarkAbilitySystemComponent* ADarkPlayerController::GetDarkAbilitySystemComponent() const
{
	const auto* DarkPS = GetDarkPlayerState();
	return (DarkPS ? DarkPS->GetDarkAbilitySystemComponent() : nullptr);
}

ADarkHUD* ADarkPlayerController::GetDarkHUD() const
{
	return CastChecked<ADarkHUD>(GetHUD(), ECastCheckedType::NullAllowed);
}

void ADarkPlayerController::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ADarkPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ADarkPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ADarkPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ADarkPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ADarkPlayerController::OnUnPossess()
{
	// Make sure the pawn that is being unpossessed doesn't remain our ASC's avatar actor
	if (APawn* PawnBeingUnpossessed = GetPawn())
	{
		const APlayerState* ThePlayerState = PlayerState.Get();
		if (IsValid(ThePlayerState))
		{
			if (auto* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(ThePlayerState))
			{
				if (ASC->GetAvatarActor() == PawnBeingUnpossessed)
				{
					ASC->SetAvatarActor(nullptr);
				}
			}
		}
	}
	
	Super::OnUnPossess();
}

void ADarkPlayerController::InitPlayerState()
{
	Super::InitPlayerState();
	
	BroadcastOnPlayerStateChanged();
}

void ADarkPlayerController::CleanupPlayerState()
{
	Super::CleanupPlayerState();
	
	BroadcastOnPlayerStateChanged();
}

void ADarkPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	BroadcastOnPlayerStateChanged();
	
	// When we're a client connected to a remote server, the player controller may replicate later than the PlayerState and AbilitySystemComponent.
	// However, TryActivateAbilitiesOnSpawn depends on the player controller being replicated in order to check whether on-spawn abilities should
	// execute locally. Therefore once the PlayerController exists and has resolved the PlayerState, try once again to activate on-spawn abilities.
	// On other net modes the PlayerController will never replicate late, so DarkASC's own TryActivateAbilitiesOnSpawn calls will succeed. The handling 
	// here is only for when the PlayerState and ASC replicated before the PC and incorrectly thought the abilities were not for the local player.
	if (GetWorld()->IsNetMode(NM_Client))
	{
		if (auto* DarkPS = GetPlayerState<ADarkPlayerState>())
		{
			if (UDarkAbilitySystemComponent* DarkASC = DarkPS->GetDarkAbilitySystemComponent())
			{
				DarkASC->RefreshAbilityActorInfo();
				DarkASC->TryActivateAbilitiesOnSpawn();
			}
		}
	}
}

void ADarkPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
}

void ADarkPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
}

void ADarkPlayerController::SetPlayer(UPlayer* InPlayer)
{
	Super::SetPlayer(InPlayer);
	
	//@Eric TODO: Uncomment after implementing UDarkLocalPlayer and UDarkSettingsShared
	// if (const auto* DarkLocalPlayer = Cast<UDarkLocalPlayer>(InPlayer))
	// {
	// 	UDarkSettingsShared* UserSettings = DarkLocalPlayer->GetSharedSettings();
	// 	UserSettings->OnSettingChanged.AddUObject(this, &ThisClass::OnSettingsChanged);
	//
	// 	OnSettingsChanged(UserSettings);
	// }
}

void ADarkPlayerController::AddCheats(bool bForce)
{
	Super::AddCheats(bForce);
}

void ADarkPlayerController::UpdateForceFeedback(IInputInterface* InputInterface, const int32 ControllerId)
{
	Super::UpdateForceFeedback(InputInterface, ControllerId);
}

void ADarkPlayerController::UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents)
{
	Super::UpdateHiddenComponents(ViewLocation, OutHiddenComponents);
}

void ADarkPlayerController::PreProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PreProcessInput(DeltaTime, bGamePaused);
}

void ADarkPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (auto* DarkASC = GetDarkAbilitySystemComponent())
	{
		DarkASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}
	
	Super::PostProcessInput(DeltaTime, bGamePaused);
}

void ADarkPlayerController::OnPlayerStateChanged()
{
	// Empty, place for derived classes to implement without having to hook all the other events
}

void ADarkPlayerController::BroadcastOnPlayerStateChanged()
{
	OnPlayerStateChanged();
}
