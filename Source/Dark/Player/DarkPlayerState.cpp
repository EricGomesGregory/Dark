// Fill out your copyright notice in the Description page of Project Settings.


#include "DarkPlayerState.h"

#include "DarkPlayerController.h"
#include "Dark/AbilitySystem/DarkAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DarkPlayerState)


ADarkPlayerState::ADarkPlayerState(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UDarkAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	// These attribute sets will be detected by AbilitySystemComponent::InitializeComponent. Keeping a reference so that the sets don't get garbage collected before that.
	//HealthSet = CreateDefaultSubobject<UDarkHealthSet>(TEXT("HealthSet"));
	
	// AbilitySystemComponent needs to be updated at a high frequency.
	SetNetUpdateFrequency(100.0f);
}

void ADarkPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MyPlayerConnectionType, SharedParams);
	
	SharedParams.Condition = ELifetimeCondition::COND_SkipOwner;
}

ADarkPlayerController* ADarkPlayerState::GetDarkPlayerController() const
{
	return Cast<ADarkPlayerController>(GetOwner());
}

UAbilitySystemComponent* ADarkPlayerState::GetAbilitySystemComponent() const
{
	return GetDarkAbilitySystemComponent();
}

void ADarkPlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ADarkPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());
}

void ADarkPlayerState::Reset()
{
	Super::Reset();
}

void ADarkPlayerState::ClientInitialize(AController* C)
{
	Super::ClientInitialize(C);
	
	//@Eric TODO: Uncomment after implementing UDarkPawnExtensionComponent
	//if (auto* PawnExtComp = UDarkPawnExtensionComponent::FindPawnExtensionComponent(GetPawn()))
	//{
	//	PawnExtComp->CheckDefaultInitialization();
	//}
}

void ADarkPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
	
	//@Eric TODO: Copy stats
}

void ADarkPlayerState::OnDeactivated()
{
	bool bDestroyDeactivatedPlayerState = false;

	switch (GetPlayerConnectionType())
	{
	case EDarkPlayerConnectionType::Player:
	case EDarkPlayerConnectionType::InactivePlayer:
		//@TODO: Ask the experience if we should destroy disconnecting players immediately or leave them around
		// (e.g., for long running servers where they might build up if lots of players cycle through)
		bDestroyDeactivatedPlayerState = true;
		break;
	default:
		bDestroyDeactivatedPlayerState = true;
		break;
	}

	SetPlayerConnectionType(EDarkPlayerConnectionType::InactivePlayer);

	if (bDestroyDeactivatedPlayerState)
	{
		Destroy();
	}
}

void ADarkPlayerState::OnReactivated()
{
	if (GetPlayerConnectionType() == EDarkPlayerConnectionType::InactivePlayer)
	{
		SetPlayerConnectionType(EDarkPlayerConnectionType::Player);
	}
}

void ADarkPlayerState::SetPlayerConnectionType(EDarkPlayerConnectionType NewType)
{
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, MyPlayerConnectionType, this);
	MyPlayerConnectionType = NewType;
}
