// Fill out your copyright notice in the Description page of Project Settings.


#include "DarkCharacter.h"

#include "DarkCharacterMovementComponent.h"
#include "DarkHealthComponent.h"
#include "DarkPawnExtensionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Dark/DarkGameplayTags.h"
#include "Dark/AbilitySystem/DarkAbilitySystemComponent.h"
#include "Dark/Camera/DarkCameraComponent.h"
#include "Dark/Player/DarkPlayerController.h"
#include "Dark/Player/DarkPlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DarkCharacter)


static FName NAME_DarkCharacterCollisionProfile_Capsule(TEXT("DarkPawnCapsule"));
static FName NAME_DarkCharacterCollisionProfile_Mesh(TEXT("DarkPawnMesh"));

ADarkCharacter::ADarkCharacter(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer.SetDefaultSubobjectClass<UDarkCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Avoid ticking characters if possible.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SetNetCullDistanceSquared(900000000.0f);

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->InitCapsuleSize(40.0f, 90.0f);
	CapsuleComp->SetCollisionProfileName(NAME_DarkCharacterCollisionProfile_Capsule);

	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));  // Rotate mesh to be X forward since it is exported as Y forward.
	MeshComp->SetCollisionProfileName(NAME_DarkCharacterCollisionProfile_Mesh);

	auto* DarkMoveComp = CastChecked<UDarkCharacterMovementComponent>(GetCharacterMovement());
	DarkMoveComp->GravityScale = 1.0f;
	DarkMoveComp->MaxAcceleration = 2400.0f;
	DarkMoveComp->BrakingFrictionFactor = 1.0f;
	DarkMoveComp->BrakingFriction = 6.0f;
	DarkMoveComp->GroundFriction = 8.0f;
	DarkMoveComp->BrakingDecelerationWalking = 1400.0f;
	DarkMoveComp->bUseControllerDesiredRotation = false;
	DarkMoveComp->bOrientRotationToMovement = false;
	DarkMoveComp->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	DarkMoveComp->bAllowPhysicsRotationDuringAnimRootMotion = false;
	DarkMoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;
	DarkMoveComp->bCanWalkOffLedgesWhenCrouching = true;
	DarkMoveComp->SetCrouchedHalfHeight(65.0f);

	PawnExtComponent = CreateDefaultSubobject<UDarkPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	PawnExtComponent->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
	PawnExtComponent->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));

	HealthComponent = CreateDefaultSubobject<UDarkHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
	HealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);

	CameraComponent = CreateDefaultSubobject<UDarkCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetRelativeLocation(FVector(-300.0f, 0.0f, 75.0f));

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	BaseEyeHeight = 80.0f;
	CrouchedEyeHeight = 50.0f;
}

ADarkPlayerController* ADarkCharacter::GetDarkPlayerController() const
{
	return Cast<ADarkPlayerController>(GetController());
}

ADarkPlayerState* ADarkCharacter::GetDarkPlayerState() const
{
	return CastChecked<ADarkPlayerState>(GetPlayerState(), ECastCheckedType::NullAllowed);
}

UDarkAbilitySystemComponent* ADarkCharacter::GetDarkAbilitySystemComponent() const
{
	return Cast<UDarkAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* ADarkCharacter::GetAbilitySystemComponent() const
{
	if (PawnExtComponent == nullptr)
	{
		return nullptr;
	}

	return PawnExtComponent->GetDarkAbilitySystemComponent();
}

void ADarkCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (const auto* DarkASC = GetDarkAbilitySystemComponent())
	{
		DarkASC->GetOwnedGameplayTags(TagContainer);
	}
}

bool ADarkCharacter::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (const auto* DarkASC = GetDarkAbilitySystemComponent())
	{
		return DarkASC->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

bool ADarkCharacter::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UDarkAbilitySystemComponent* DarkASC = GetDarkAbilitySystemComponent())
	{
		return DarkASC->HasAllMatchingGameplayTags(TagContainer);
	}

	return false;
}

bool ADarkCharacter::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const auto* DarkASC = GetDarkAbilitySystemComponent())
	{
		return DarkASC->HasAnyMatchingGameplayTags(TagContainer);
	}

	return false;
}

void ADarkCharacter::ToggleCrouch()
{
	const auto* DarkMoveComp = CastChecked<UDarkCharacterMovementComponent>(GetCharacterMovement());
	
	if (IsCrouched() || DarkMoveComp->bWantsToCrouch)
	{
		UnCrouch();
	}
	else if (DarkMoveComp->IsMovingOnGround())
	{
		Crouch();
	}
}

void ADarkCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ADarkCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ADarkCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ADarkCharacter::Reset()
{
	DisableMovementAndCollision();

	K2_OnReset();

	UninitAndDestroy();
}

void ADarkCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ADarkCharacter::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);
}

void ADarkCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();
}

void ADarkCharacter::OnAbilitySystemInitialized()
{
	UDarkAbilitySystemComponent* DarkASC = GetDarkAbilitySystemComponent();
	check(DarkASC);

	HealthComponent->InitializeWithAbilitySystem(DarkASC);

	InitializeGameplayTags();
}

void ADarkCharacter::OnAbilitySystemUninitialized()
{
	HealthComponent->UninitializeFromAbilitySystem();
}

void ADarkCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	PawnExtComponent->HandleControllerChanged();
}

void ADarkCharacter::UnPossessed()
{
	Super::UnPossessed();
	
	PawnExtComponent->HandleControllerChanged();
}

void ADarkCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();
	
	PawnExtComponent->HandleControllerChanged();
}

void ADarkCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	PawnExtComponent->HandlePlayerStateReplicated();
}

void ADarkCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PawnExtComponent->SetupPlayerInputComponent();
}

void ADarkCharacter::InitializeGameplayTags()
{
	// Clear tags that may be lingering on the ability system from the previous pawn.
	if (UDarkAbilitySystemComponent* DarkASC = GetDarkAbilitySystemComponent())
	{
		for (const TPair<uint8, FGameplayTag>& TagMapping : DarkGameplayTags::MovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				DarkASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		for (const TPair<uint8, FGameplayTag>& TagMapping : DarkGameplayTags::CustomMovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				DarkASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		auto* DarkMoveComp = CastChecked<UDarkCharacterMovementComponent>(GetCharacterMovement());
		SetMovementModeTag(DarkMoveComp->MovementMode, DarkMoveComp->CustomMovementMode, true);
	}
}

void ADarkCharacter::FellOutOfWorld(const class UDamageType& dmgType)
{
	HealthComponent->DamageSelfDestruct(/*bFellOutOfWorld=*/ true);
}

void ADarkCharacter::OnDeathStarted(AActor* OwningActor)
{
	DisableMovementAndCollision();
}

void ADarkCharacter::OnDeathFinished(AActor* OwningActor)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyDueToDeath);
}

void ADarkCharacter::DisableMovementAndCollision()
{
	if (GetController())
	{
		GetController()->SetIgnoreMoveInput(true);
	}

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	auto* DarkMoveComp = CastChecked<UDarkCharacterMovementComponent>(GetCharacterMovement());
	DarkMoveComp->StopMovementImmediately();
	DarkMoveComp->DisableMovement();
}

void ADarkCharacter::DestroyDueToDeath()
{
	K2_OnDeathFinished();

	UninitAndDestroy();
}

void ADarkCharacter::UninitAndDestroy()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		DetachFromControllerPendingDestroy();
		SetLifeSpan(0.1f);
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	if (UDarkAbilitySystemComponent* DarkASC = GetDarkAbilitySystemComponent())
	{
		if (DarkASC->GetAvatarActor() == this)
		{
			PawnExtComponent->UninitializeAbilitySystem();
		}
	}

	SetActorHiddenInGame(true);
}

void ADarkCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	
	auto* DarkMoveComp = CastChecked<UDarkCharacterMovementComponent>(GetCharacterMovement());

	SetMovementModeTag(PrevMovementMode, PreviousCustomMode, false);
	SetMovementModeTag(DarkMoveComp->MovementMode, DarkMoveComp->CustomMovementMode, true);
}

void ADarkCharacter::SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled)
{
	if (UDarkAbilitySystemComponent* DarkASC = GetDarkAbilitySystemComponent())
	{
		const FGameplayTag* MovementModeTag = nullptr;
		if (MovementMode == MOVE_Custom)
		{
			MovementModeTag = DarkGameplayTags::CustomMovementModeTagMap.Find(CustomMovementMode);
		}
		else
		{
			MovementModeTag = DarkGameplayTags::MovementModeTagMap.Find(MovementMode);
		}

		if (MovementModeTag && MovementModeTag->IsValid())
		{
			DarkASC->SetLooseGameplayTagCount(*MovementModeTag, (bTagEnabled ? 1 : 0));
		}
	}
}

void ADarkCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (UDarkAbilitySystemComponent* DarkASC = GetDarkAbilitySystemComponent())
	{
		DarkASC->SetLooseGameplayTagCount(DarkGameplayTags::Status_Crouching, 1);
	}
	
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void ADarkCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (UDarkAbilitySystemComponent* DarkASC = GetDarkAbilitySystemComponent())
	{
		DarkASC->SetLooseGameplayTagCount(DarkGameplayTags::Status_Crouching, 0);
	}

	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

bool ADarkCharacter::CanJumpInternal_Implementation() const
{
	return Super::CanJumpInternal_Implementation();
}
