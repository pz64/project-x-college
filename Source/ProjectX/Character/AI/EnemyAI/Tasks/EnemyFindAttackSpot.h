// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Runtime/AIModule/Classes/EnvironmentQuery/EnvQueryManager.h"
#include "EnemyFindAttackSpot.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTX_API UEnemyFindAttackSpot : public UBTTaskNode
{
	GENERATED_BODY()
	
protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	UPROPERTY()
		class UEnvQuery* Query;

	FEnvQueryRequest QueryRequest;

	UPROPERTY()
		class AProjectXAIController* AIController;

	UPROPERTY()
		class AProjectXAICharacter* AI;

public:

	UEnemyFindAttackSpot();

	// The function that gets called when query is finished
	void QueryFinished(TSharedPtr<struct FEnvQueryResult> Result);


};

template<typename T>
void FindAllActors(UWorld* World, TArray<T*>& Out);