// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ProjectXAIController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTX_API AProjectXAIController : public AAIController
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(transient)
		class UBlackboardComponent* BlackboardComp;
	UPROPERTY(transient)
		class UBehaviorTreeComponent* BehaviourTreeComp;
	UPROPERTY(transient)
		class UAIPerceptionComponent* PerceptionComp;

public:
	FORCEINLINE class UBlackboardComponent* GetBlackboardComp() { return BlackboardComp; }

	class UAISenseConfig_Sight* Sight;

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
	
	
};
