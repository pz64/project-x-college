// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Runtime/AIModule/Classes/EnvironmentQuery/EnvQueryManager.h"
#include "FindAttackSpot.generated.h"


/**
 *
 */
UCLASS()
class PROJECTX_API UFindAttackSpot : public UBTTask_BlackboardBase
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

	UFindAttackSpot();

	// The function that gets called when query is finished
	void QueryFinished(TSharedPtr<struct FEnvQueryResult> Result);


};

template<typename T>
void FindAllActors(UWorld* World, TArray<T*>& Out);
