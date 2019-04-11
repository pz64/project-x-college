// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIStates.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EAIStates : uint8
{
	HAND_FREE,
	ARMED,
	PATROL,
	FOLLOW_PLAYER,
	COMBAT_MODE
};
