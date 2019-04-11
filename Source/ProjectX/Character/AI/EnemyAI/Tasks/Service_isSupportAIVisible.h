// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "Service_isSupportAIVisible.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTX_API UService_isSupportAIVisible : public UBTService
{
	GENERATED_BODY()
	
public:
	UService_isSupportAIVisible();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};

template<typename T>
void FindAllActors(UWorld* World, TArray<T*>& Out);
