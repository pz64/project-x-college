// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "UpdateAISpeed.generated.h"

/**
 *
 */
UCLASS()
class PROJECTX_API UUpdateAISpeed : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

private:

public:

	UUpdateAISpeed();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);


};
