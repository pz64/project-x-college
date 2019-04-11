// Fill out your copyright notice in the Description page of Project Settings.

#include "UpdateAISpeed.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Character/AI/ProjectXAIController.h"
#include "Character/AI/ProjectXAICharacter.h"
#include "Character/ProjectXCharacter.h"

UUpdateAISpeed::UUpdateAISpeed()
{
}

EBTNodeResult::Type UUpdateAISpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AProjectXAIController* AIController = Cast<AProjectXAIController>(OwnerComp.GetOwner());
	AProjectXAICharacter* AI = AIController ? Cast<AProjectXAICharacter>(AIController->GetPawn()) : nullptr;
	if (AI)
	{
		FVector PlayerLoc = AIController->GetBlackboardComp()->GetValueAsVector(FName("selfLocation"));
		if (!PlayerLoc.IsZero())
		{
			float distance = FVector::Dist(PlayerLoc, AI->GetActorLocation());

			if (distance > 2000)
				AI->Sprint();
			else if (distance > 500)
				AI->Run();
			else AI->Walk();

			return EBTNodeResult::Succeeded;
		}

	}
	return EBTNodeResult::Failed;
}


