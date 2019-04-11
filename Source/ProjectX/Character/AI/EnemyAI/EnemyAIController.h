// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AI/ProjectXAIController.h"
#include "EnemyAIController.generated.h"

/**
 *
 */
UCLASS()
class PROJECTX_API AEnemyAIController : public AProjectXAIController
{
	GENERATED_BODY()


protected:
	UFUNCTION()
		void OnPerceptionUpdated(TArray<AActor*> UpdatedActors);



public:
	AEnemyAIController();

	virtual void Possess(APawn* InPawn) override;

	uint8 SelfKeyID;


	/////////////////
	//				//
	//				//
	//    Player    //
	//				//
	//				//
	/////////////////

	uint8 PlayerKeyID;

	const FName BlackboardPlayerKey = FName("Player");

	bool isPlayerNearby();



};


template<typename T>
void FindAllActors(UWorld* World, TArray<T*>& Out);
