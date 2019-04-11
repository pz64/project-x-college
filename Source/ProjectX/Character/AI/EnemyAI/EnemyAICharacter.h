// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AI/ProjectXAICharacter.h"
#include "EnemyAICharacter.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTX_API AEnemyAICharacter : public AProjectXAICharacter
{
	GENERATED_BODY()

public:

	AEnemyAICharacter();

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	///////////////////////////
	//				          //
	//				          //
	// GroupBehavior          //
	//				          //
	//				          //
	///////////////////////////

protected:
	TArray<class AEnemyAIBehavior*> groupbehavior;
};


template<typename T>
void FindAllActors(UWorld* World, TArray<T*>& Out);
