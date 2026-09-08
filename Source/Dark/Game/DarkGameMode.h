// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DarkGameMode.generated.h"

/**
 * Post login event, triggered when a player or bot joins the game as well as after seamless and non-seamless travel
 *
 * This is called after the player has finished initialization
 */
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnDarkGameModePlayerInitialized, AGameModeBase* /*GameMode*/, AController* /*NewPlayer*/);

/**
 * ADarkGameMode
 *
 *	The base game mode class used by this project.
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The base game mode class used by this project."))
class DARK_API ADarkGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ADarkGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
