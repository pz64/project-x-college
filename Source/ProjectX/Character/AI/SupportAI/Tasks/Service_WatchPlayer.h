// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "Service_WatchPlayer.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTX_API UService_WatchPlayer : public UBTService_BlackboardBase
{
	GENERATED_BODY()


	
public:

	UService_WatchPlayer();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	

};

template<typename T>
void FindAllActors(UWorld* World, TArray<T*>& Out);
