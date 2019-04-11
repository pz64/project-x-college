// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AI/ProjectXAIController.h"
#include "SupportAIController.generated.h"

/**
 *
 */
UCLASS()
class PROJECTX_API ASupportAIController : public AProjectXAIController
{
	GENERATED_BODY()


protected:
	UFUNCTION()
		void OnPerceptionUpdated(TArray<AActor*> UpdatedActors);



public:
	ASupportAIController();

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