// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

namespace DarkGameplayTags
{
	DARK_API	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);
	
	DARK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_IsDead);
	DARK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cooldown);
	DARK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cost);
	DARK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsBlocked);
	DARK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsMissing);
	DARK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Networking);
	DARK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_ActivationGroup);

	DARK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Behavior_SurvivesDeath);

	DARK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_AbilityInputBlocked);
	DARK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_MovementStopped);
	
	DARK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Move);
	DARK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Look_Mouse);
	DARK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Look_Stick);
	DARK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Crouch);
	DARK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Jump);
	
	DARK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_Spawned);
	DARK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataAvailable);
	DARK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataInitialized);
	DARK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_GameplayReady);
	
	DARK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Crouching);
	DARK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death);
	DARK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dying);
	DARK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dead);

	// These are mappings from MovementMode enums to GameplayTags associated with those enums (below)
	DARK_API	extern const TMap<uint8, FGameplayTag> MovementModeTagMap;
	DARK_API	extern const TMap<uint8, FGameplayTag> CustomMovementModeTagMap;

	DARK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Walking);
	DARK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_NavWalking);
	DARK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Falling);
	DARK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Swimming);
	DARK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Flying);

	DARK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Custom);
}
