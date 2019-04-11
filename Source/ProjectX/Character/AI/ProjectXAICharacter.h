// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ProjectXCharacter.h"
#include "ProjectXAICharacter.generated.h"

UENUM(BlueprintType)
enum class EPlayerType : uint8
{
	SUPPORT,
	ENEMY
};

/**
 *
 */
UCLASS()
class PROJECTX_API AProjectXAICharacter : public AProjectXCharacter
{
	GENERATED_BODY()

public:

	EPlayerType PlayerType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		class UBehaviorTree* AIBehaviour;
};
