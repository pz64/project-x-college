// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AI/ProjectXAICharacter.h"
#include "SupportAICharacter.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTX_API ASupportAICharacter : public AProjectXAICharacter
{
	GENERATED_BODY()

public:

	ASupportAICharacter();

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
	TArray<class AAIBehavior*> groupbehavior;
};


template<typename T>
void FindAllActors(UWorld* World, TArray<T*>& Out);
