// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DarkGameInstance.generated.h"

class ADarkPlayerController;


/**
 * UDarkGameInstance
 * 
 *@Eric TODO: Document this
 */
UCLASS(Config = Game)
class DARK_API UDarkGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UDarkGameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	ADarkPlayerController* GetPrimaryPlayerController() const;
	
protected:
	virtual void Init() override;
	virtual void Shutdown() override;
};
