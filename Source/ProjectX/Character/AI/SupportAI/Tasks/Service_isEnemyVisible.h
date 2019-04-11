// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "Service_isEnemyVisible.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTX_API UService_isEnemyVisible : public UBTService
{
	GENERATED_BODY()

public:
	UService_isEnemyVisible();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};

template<typename T>
void FindAllActors(UWorld* World, TArray<T*>& Out);
