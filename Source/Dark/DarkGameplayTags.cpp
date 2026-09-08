// Fill out your copyright notice in the Description page of Project Settings.


#include "DarkGameplayTags.h"

namespace DarkGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_IsDead, "Ability.ActivateFail.IsDead", "Ability failed to activate because its owner is dead.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_Cooldown, "Ability.ActivateFail.Cooldown", "Ability failed to activate because it is on cool down.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_Cost, "Ability.ActivateFail.Cost", "Ability failed to activate because it did not pass the cost checks.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_TagsBlocked, "Ability.ActivateFail.TagsBlocked", "Ability failed to activate because tags are blocking it.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_TagsMissing, "Ability.ActivateFail.TagsMissing", "Ability failed to activate because tags are missing.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_Networking, "Ability.ActivateFail.Networking", "Ability failed to activate because it did not pass the network checks.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_ActivationGroup, "Ability.ActivateFail.ActivationGroup", "Ability failed to activate because of its activation group.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Behavior_SurvivesDeath, "Ability.Behavior.SurvivesDeath", "An ability with this type tag should not be canceled due to death.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_AbilityInputBlocked, "Gameplay.AbilityInputBlocked", "");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Move, "Input.Move", "Move input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Look_Mouse, "Input.Look.Mouse", "Look (mouse) input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Look_Stick, "Input.Look.Stick", "Look (stick) input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Crouch, "Input.Crouch", "Crouch input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Jump, "Input.Jump", "Jump input.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_Spawned, "InitState.Spawned", "1: Actor/component has initially spawned and can be extended");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_DataAvailable, "InitState.DataAvailable", "2: All required data has been loaded/replicated and is ready for initialization");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_DataInitialized, "InitState.DataInitialized", "3: The available data has been initialized for this actor/component, but it is not ready for full gameplay");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_GameplayReady, "InitState.GameplayReady", "4: The actor/component is fully ready for active gameplay");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Crouching, "Status.Crouching", "Target is crouching.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Death, "Status.Death", "Target has the death status.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Death_Dying, "Status.Death.Dying", "Target has begun the death process.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Death_Dead, "Status.Death.Dead", "Target has finished the death process.");
	
}
