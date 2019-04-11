// Fill out your copyright notice in the Description page of Project Settings.

#include "SupportAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "SupportAICharacter.h"
#include "Character/ProjectXPlayerCharacter.h"
#include "Character/AI/CombatBehavior.h"
#include "Character/AI/EnemyAI/EnemyAICharacter.h"
#include "EngineUtils.h"

ASupportAIController::ASupportAIController()
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

void ASupportAIController::Possess(APawn * InPawn)
{
	Super::Possess(InPawn);
	ASupportAICharacter *AIChar = Cast<ASupportAICharacter>(InPawn);
	if (AIChar && AIChar->AIBehaviour)
	{
		BlackboardComp->InitializeBlackboard(*AIChar->AIBehaviour->BlackboardAsset);
		SelfKeyID = BlackboardComp->GetKeyID("SelfActor");
		PlayerKeyID = BlackboardComp->GetKeyID("Player");
		BlackboardComp->SetValue<UBlackboardKeyType_Object>(SelfKeyID, AIChar);
		BehaviourTreeComp->StartTree(*AIChar->AIBehaviour);

		TArray<AProjectXPlayerCharacter*> Player;
		FindAllActors<AProjectXPlayerCharacter>(GetWorld(), Player);
		BlackboardComp->SetValueAsObject(FName("Player"), Player[0]);

	}
	GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &ASupportAIController::OnPerceptionUpdated);
}


bool ASupportAIController::isPlayerNearby()
{

	return BlackboardComp->GetValue<UBlackboardKeyType_Object>(PlayerKeyID) ? true : false;
}

void ASupportAIController::OnPerceptionUpdated(TArray<AActor*> UpdatedActors)
{
	TArray<ACombatBehavior*> CombatBehavior;
	FindAllActors<ACombatBehavior>(GetWorld(), CombatBehavior);

	bool EnemyDetected = false;
	for (AActor* Player : UpdatedActors)
	{

		if (
			CombatBehavior.Num() &&
			Player->IsA<AEnemyAICharacter>() &&
			Cast<AEnemyAICharacter>(Player) == CombatBehavior[0]->GetEnemyAIOf(Cast<ASupportAICharacter>(GetPawn()))
			)
		{
			EnemyDetected = true;
			BlackboardComp->SetValueAsObject("Enemy", Player);
			break;
		}
	}


	for (AActor* Player : UpdatedActors)
	{
		if (Player->IsA<AProjectXPlayerCharacter>() && !isPlayerNearby())
		{
			BlackboardComp->SetValueAsObject(BlackboardPlayerKey, Player);
			break;
		}
	}

	if (!isPlayerNearby())
		BlackboardComp->SetValueAsObject(BlackboardPlayerKey, nullptr);
}









