// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Logging/LogMacros.h"


DARK_API DECLARE_LOG_CATEGORY_EXTERN(LogDark, Log, All);
DARK_API DECLARE_LOG_CATEGORY_EXTERN(LogDarkAbilitySystem, Log, All);

DARK_API FString GetClientServerContextString(UObject* ContextObject = nullptr);
