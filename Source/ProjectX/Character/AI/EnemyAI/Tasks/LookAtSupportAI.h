// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "Runtime/AIModule/Classes/EnvironmentQuery/EnvQueryManager.h"
#include "LookAtSupportAI.generated.h"

/**
 *
 */
UCLASS()
class PROJECTX_API ULookAtSupportAI : public UBTService_BlackboardBase
{
	GENERATED_BODY()
protected:

	class ASupportAICharacter* SupportAI;

	class AEnemyAIController* AIController;

	class AEnemyAICharacter* AI;

public:

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	ULookAtSupportAI();

protected:

	UPROPERTY()
		class UEnvQuery* Query;

	FEnvQueryRequest QueryRequest;

	// The function that gets called when query is finished
public:
	void QueryFinished(TSharedPtr<struct FEnvQueryResult> Result);


};

