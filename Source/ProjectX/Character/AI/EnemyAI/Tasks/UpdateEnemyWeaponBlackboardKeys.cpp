// Fill out your copyright notice in the Description page of Project Settings.

#include "UpdateEnemyWeaponBlackboardKeys.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Character/AI/EnemyAI/EnemyAIController.h"
#include "Character/AI/EnemyAI/EnemyAICharacter.h"
#include "Character/ProjectXCharacter.h"
#include "Character/AI/EnemyAI/EnemyAIBehavior.h"
#include "Interactable/Weapon/Weapon.h"
#include "EngineUtils.h"


EBTNodeResult::Type UUpdateEnemyWeaponBlackboardKeys::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyAIController* AIController = Cast<AEnemyAIController>(OwnerComp.GetOwner());
	AEnemyAICharacter* AI = AIController ? Cast<AEnemyAICharacter>(AIController->GetPawn()) : nullptr;
	TArray<AEnemyAIBehavior*> Behavior;
	FindAllActors<AEnemyAIBehavior>(GetWorld(), Behavior);
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