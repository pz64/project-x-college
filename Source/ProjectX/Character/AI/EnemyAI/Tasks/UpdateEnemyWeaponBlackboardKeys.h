// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "UpdateEnemyWeaponBlackboardKeys.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTX_API UUpdateEnemyWeaponBlackboardKeys : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

template<typename T>
void FindAllActors(UWorld* World, TArray<T*>& Out);