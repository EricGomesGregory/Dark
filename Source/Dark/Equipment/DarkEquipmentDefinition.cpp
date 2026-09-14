// Fill out your copyright notice in the Description page of Project Settings.


#include "DarkEquipmentDefinition.h"

#include "DarkEquipmentInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DarkEquipmentDefinition)


UDarkEquipmentDefinition::UDarkEquipmentDefinition(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	InstanceType = UDarkEquipmentInstance::StaticClass();
}
