// Fill out your copyright notice in the Description page of Project Settings.

#include "LookAtSupportAI.h"
#include "Character/AI/CombatBehavior.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Character/AI/EnemyAI/EnemyAICharacter.h"
#include "Character/AI/EnemyAI/EnemyAIController.h"
#include "Character/AI/SupportAI/SupportAICharacter.h"
#include "Character/ProjectXPlayerCharacter.h"
#include "Character/AI/AIStates.h"
#include "EngineUtils.h"
#include "Interactable/HideSpots/HideSpotData.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Interactable/Weapon/Weapon.h"
#include "Runtime/AIModule/Classes/EnvironmentQuery/EnvQuery.h"
#include "Runtime/AIModule/Classes/EnvironmentQuery/EnvQueryTypes.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

ULookAtSupportAI::ULookAtSupportAI()
{
	static ConstructorHelpers::FObjectFinder<UEnvQuery> EQS_Query(TEXT("EnvQuery'/Game/Game/Characters/AI/AI_Enemy/EnvQueries/EQ_EnemyRandomMove'"));
	Query = EQS_Query.Object;

	if (Query != nullptr)
	{
		QueryRequest = FEnvQueryRequest(Query, this);
	}
}

void ULookAtSupportAI::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AIController = Cast<AEnemyAIController>(OwnerComp.GetOwner());
	AI = AIController ? Cast<AEnemyAICharacter>(AIController->GetPawn()) : nullptr;
	AProjectXPlayerCharacter* Player = Cast<AProjectXPlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());


	if (AIController->GetBlackboardComp()->GetValueAsEnum("AIStates") == static_cast<uint8>(EAIStates::HAND_FREE))
		AIController->GetBlackboardComp()->SetValueAsEnum(FName("AIStates"), static_cast<uint8>(EAIStates::PATROL));

	if (Player)
	{
		AIController->GetBlackboardComp()->SetValueAsEnum(FName("AIStates"), static_cast<uint8>(EAIStates::COMBAT_MODE));
		AIController->GetBlackboardComp()->SetValueAsVector(FName("PlayerLocation"), Player->GetActorLocation());
		AIController->GetBlackboardComp()->SetValueAsObject(FName("Enemy"), Player);
	}
		

	//FIND DISTANCE B/W EVERY ENEMY AND CURRENT AI. SELECT ONE ENEMY WITH FAVOURABLE VALUE. 
	TArray<ASupportAICharacter*> SupportAIs;
	TArray<float> Distances;

	FindAllActors<ASupportAICharacter>(GetWorld(), SupportAIs);
	for (int32 i = 0; i < SupportAIs.Num(); ++i) {
		FVector src = AI->HoldingWeapon->meshComponent->GetSocketLocation("Muzzle");
		FVector dst = SupportAIs[i]->GetActorLocation();

		FHitResult HitResult(ForceInit);

		FCollisionQueryParams traceqparam;
		traceqparam.AddIgnoredComponent(AI->GetCapsuleComponent());
		traceqparam.AddIgnoredActor(AI);
		traceqparam.AddIgnoredActor(AI->HoldingWeapon);

		bool successfulHit = GetWorld()->LineTraceSingleByChannel(HitResult, src, dst, ECollisionChannel::ECC_Camera, traceqparam);

		if (successfulHit && HitResult.GetActor() && HitResult.GetActor()->IsA<ASupportAICharacter>())
		{
			Distances.Add(FVector::Dist(src, dst));
		}
		else
		{
			Distances.Add(-1);
		}
	}

	ASupportAICharacter* SupportAI = Cast<ASupportAICharacter>(AIController->GetBlackboardComp()->GetValueAsObject(FName("Enemy")));
	if (!SupportAI || (SupportAI && SupportAI->IsDead())) {
		float smallest = 99999999.0;
		for (int32 i = 0; i < Distances.Num(); ++i)
		{
			if (smallest > Distances[i] && !SupportAIs[i]->IsDead())
			{
				smallest = Distances[i];
				SupportAI = SupportAIs[i];
			}
		}
	}

	EAIStates currentState = static_cast<EAIStates>(AIController->GetBlackboardComp()->GetValueAsEnum(FName("AIStates")));

	AProjectXCharacter* Character = nullptr;
	
	if (SupportAI && !Player)
		Character = SupportAI;
	else if (Player && !SupportAI)
		Character = Player;
	else if(Player && SupportAI)
		Character = (FVector::Dist(AI->GetActorLocation(), SupportAI->GetActorLocation()) < FVector::Dist(AI->GetActorLocation(), Player->GetActorLocation())) ? Cast<AProjectXCharacter>(SupportAI) : Cast<AProjectXCharacter>(Player);
	
	if (Character)
	{

		AIController->GetBlackboardComp()->SetValueAsEnum(FName("AIStates"), static_cast<uint8>(EAIStates::COMBAT_MODE));
		AIController->SetFocus(Cast<AActor>(Character));
		AIController->GetBlackboardComp()->SetValueAsObject(FName("Enemy"), Character);

		if (FVector::Dist(AI->GetActorLocation(), Character->GetActorLocation()) < 1500.0f)
		{
			FVector Location = Character->GetActorLocation();

			QueryRequest.Execute(EEnvQueryRunMode::RandomBest5Pct, this, &ULookAtSupportAI::QueryFinished);
		}
		else
		{
			FVector Location = Character->GetActorLocation();

			QueryRequest.Execute(EEnvQueryRunMode::RandomBest25Pct, this, &ULookAtSupportAI::QueryFinished);
		}
	} 
	else
	{
		AIController->GetBlackboardComp()->SetValueAsObject(FName("Enemy"), nullptr);
		AIController->GetBlackboardComp()->SetValueAsEnum(FName("AIStates"), static_cast<uint8>(EAIStates::PATROL));
	}
}

void ULookAtSupportAI::QueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
	FVector loc = Result.Get()->GetItemAsLocation(0);
	if (loc != FVector(0, 0, 0))
		AIController->GetBlackboardComp()->SetValueAsVector(FName("selfLocation"),loc);
	else
		AIController->GetBlackboardComp()->SetValueAsVector(FName("selfLocation"), loc);
	
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

