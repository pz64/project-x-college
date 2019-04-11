// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "enemyAICharacter.h"
#include "Character/ProjectXPlayerCharacter.h"
#include "EngineUtils.h"
#include "Character/AI/SupportAI/SupportAICharacter.h"
#include "Character/AI/CombatBehavior.h"


AEnemyAIController::AEnemyAIController()
{
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
	BehaviourTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviourTreeComp"));
	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
	Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight"));

	Sight->SightRadius = 4000.f;
	Sight->LoseSightRadius = 4500.f;
	Sight->PeripheralVisionAngleDegrees = 170.f;

	//Tell the sight sense to detect everything
	Sight->DetectionByAffiliation.bDetectEnemies = true;
	Sight->DetectionByAffiliation.bDetectFriendlies = true;
	Sight->DetectionByAffiliation.bDetectNeutrals = true;

	SetPerceptionComponent(*PerceptionComponent);
	PerceptionComp->ConfigureSense(*Sight);



}

void AEnemyAIController::Possess(APawn * InPawn)
{
	Super::Possess(InPawn);
	AEnemyAICharacter *AIChar = Cast<AEnemyAICharacter>(InPawn);
	if (AIChar && AIChar->AIBehaviour)
	{
		BlackboardComp->InitializeBlackboard(*AIChar->AIBehaviour->BlackboardAsset);
		SelfKeyID = BlackboardComp->GetKeyID("SelfActor");
		PlayerKeyID = BlackboardComp->GetKeyID("Player");
		BlackboardComp->SetValue<UBlackboardKeyType_Object>(SelfKeyID, AIChar);
		BehaviourTreeComp->StartTree(*AIChar->AIBehaviour);
	}
	GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnPerceptionUpdated);
}


bool AEnemyAIController::isPlayerNearby()
{

	return BlackboardComp->GetValue<UBlackboardKeyType_Object>(PlayerKeyID) ? true : false;
}

void AEnemyAIController::OnPerceptionUpdated(TArray<AActor*> UpdatedActors)
{
	TArray<ACombatBehavior*> CombatBehavior;
	FindAllActors<ACombatBehavior>(GetWorld(), CombatBehavior);

	bool EnemyDetected = false;
	for (AActor* Player : UpdatedActors)
	{

		if (
			CombatBehavior.Num() &&
			Player->IsA<ASupportAICharacter>() &&
			Cast<ASupportAICharacter>(Player) == CombatBehavior[0]->GetSupportAIOf(Cast<AEnemyAICharacter>(GetPawn()))
			)
		{
			EnemyDetected = true;
			BlackboardComp->SetValueAsObject("Enemy", Player);
			break;
		}
	}

	for (AActor* Player : UpdatedActors)
		if (Player->IsA<AProjectXPlayerCharacter>() && !isPlayerNearby())
		{
			BlackboardComp->SetValueAsObject(BlackboardPlayerKey, Player);
			return;
		}
	if (!isPlayerNearby())
		BlackboardComp->SetValueAsObject(BlackboardPlayerKey, nullptr);
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







