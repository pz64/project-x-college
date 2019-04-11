// Fill out your copyright notice in the Description page of Project Settings.

#include "UpdateWeaponBlackboardKeys.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Character/AI/SupportAI//SupportAIController.h"
#include "Character/AI/SupportAI/SupportAICharacter.h"
#include "Character/ProjectXCharacter.h"
#include "Character/AI/SupportAI/AIBehavior.h"
#include "Interactable/Weapon/Weapon.h"
#include "EngineUtils.h"


EBTNodeResult::Type UUpdateWeaponBlackboardKeys::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ASupportAIController* AIController = Cast<ASupportAIController>(OwnerComp.GetOwner());
	ASupportAICharacter* AI = AIController ? Cast<ASupportAICharacter>(AIController->GetPawn()) : nullptr;
	TArray<AAIBehavior*> Behavior;
	FindAllActors<AAIBehavior>(GetWorld(), Behavior);
	if (Behavior.Num() > 0 && AI)
	{
		AWeapon* Weapon = Behavior[0]->GetWeaponAssignedToCharacter(AI);
		AWeapon* Inventory = Behavior[0]->GetInventoryAssignedToCharacter(AI);
		AIController->GetBlackboardComp()->SetValueAsObject(FName("Weapon"), Weapon);
		AIController->GetBlackboardComp()->SetValueAsObject(FName("Inventory"), Inventory);

		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}


template<typename T>
void FindAllActors(UWorld* World, TArray<T*>& Out)
{
	for (TActorIterator<AActor> It(World, T::StaticClass()); It; ++It)
	{
		T* Actor = Cast<T>(*It);
		if (Actor && !Actor->IsPendingKill())
		{
			Out.Add(Actor);
		}
	}
}