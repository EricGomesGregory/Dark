// Fill out your copyright notice in the Description page of Project Settings.


#include "DarkGameMode.h"

#include "Dark/Player/DarkPlayerController.h"
#include "Dark/Character/DarkCharacter.h"
#include "Dark/Player/DarkPlayerState.h"
#include "Dark/UI/DarkHUD.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DarkGameMode)


ADarkGameMode::ADarkGameMode(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	//GameStateClass = ADarkGameState::StaticClass();
	//GameSessionClass = ADarkGameSession::StaticClass();
	PlayerControllerClass = ADarkPlayerController::StaticClass();
	//ReplaySpectatorPlayerControllerClass = ADarkReplayPlayerController::StaticClass();
	PlayerStateClass = ADarkPlayerState::StaticClass();
	DefaultPawnClass = ADarkCharacter::StaticClass();
	HUDClass = ADarkHUD::StaticClass();
}
