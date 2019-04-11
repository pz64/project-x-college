// Fill out your copyright notice in the Description page of Project Settings.
#include "Service_WatchPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Character/AI/ProjectXAIController.h"
#include "Character/AI/ProjectXAICharacter.h"
#include "Character/ProjectXPlayerCharacter.h"
#include "Character/AI/SupportAI/AIBehavior.h"
#include "Character/AI/EnemyAI/EnemyAICharacter.h"
#include "Interactable/Weapon/Weapon.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Character/AI/CombatBehavior.h"
#include "Interactable/HideSpots/HideSpotData.h"
#include "Character/AI/AIStates.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "EngineUtils.h"


UService_WatchPlayer::UService_WatchPlayer()
{
	bCreateNodeInstance = true;
}

void UService_WatchPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AProjectXAIController* AIController = Cast<AProjectXAIController>(OwnerComp.GetOwner());
	AProjectXAICharacter* AI = AIController ? Cast<AProjectXAICharacter>(AIController->GetPawn()) : nullptr;
	AProjectXPlayerCharacter* Player = Cast<AProjectXPlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

	if (Player)
		AIController->GetBlackboardComp()->SetValueAsVector(FName("PlayerLocation"), Player->GetActorLocation());


	if (AI && Player)
	{

		//FIND DISTANCE B/W EVERY ENEMY AND CURRENT AI. SELECT ONE ENEMY WITH FAVOURABLE VALUE. 
		TArray<AEnemyAICharacter*> EnemyAIs;
		TArray<float> Distances;

		FindAllActors<AEnemyAICharacter>(GetWorld(), EnemyAIs);
		for (int32 i = 0; i < EnemyAIs.Num(); ++i) {
			FVector src = AI->HoldingWeapon->meshComponent->GetSocketLocation("Muzzle");
			FVector dst = EnemyAIs[i]->GetActorLocation();

			FHitResult HitResult(ForceInit);

			FCollisionQueryParams traceqparam;
			traceqparam.AddIgnoredComponent(AI->GetCapsuleComponent());
			traceqparam.AddIgnoredActor(AI);
			traceqparam.AddIgnoredActor(AI->HoldingWeapon);

			bool successfulHit = GetWorld()->LineTraceSingleByChannel(HitResult, src, dst, ECollisionChannel::ECC_Camera, traceqparam);

			if (successfulHit && HitResult.GetActor() && HitResult.GetActor()->IsA<AEnemyAICharacter>())
			{
				Distances.Add(FVector::Dist(src, dst));
			}
			else
			{
				Distances.Add(-1);
			}
		}

		AEnemyAICharacter* EnemyAI = Cast<AEnemyAICharacter>(AIController->GetBlackboardComp()->GetValueAsObject(FName("Enemy")));
		if (!EnemyAI || (EnemyAI && EnemyAI->IsDead())) {
			float smallest = 99999999.0;
			for (int32 i = 0; i < Distances.Num(); ++i)
			{
				if (smallest > Distances[i] && !EnemyAIs[i]->IsDead())
				{
					smallest = Distances[i];
					EnemyAI = EnemyAIs[i];
				}
			}
		}

		EAIStates currentState = static_cast<EAIStates>(AIController->GetBlackboardComp()->GetValueAsEnum(FName("AIStates")));

		if (EnemyAI)
		{
			AIController->GetBlackboardComp()->SetValueAsObject(FName("Enemy"), EnemyAI);
			AIController->SetFocus(EnemyAI, EAIFocusPriority::Gameplay);
			AIController->GetBlackboardComp()->SetValueAsEnum(FName("AIStates"), static_cast<uint8>(EAIStates::COMBAT_MODE));
		}

		TArray<AHideSpotData*> HideSpots;
		FindAllActors<AHideSpotData>(GetWorld(), HideSpots);
		if (HideSpots.Num())
		{
			for (int32 i = 0; i < HideSpots.Num(); ++i)
			{
				if (HideSpots[i]->AllocatedCharacter == AI)
				{
					float PlayerSpotDist = FVector::Dist(Player->GetActorLocation(), HideSpots[i]->GetActorLocation());
					if (PlayerSpotDist > 2000.f)
					{
						AIController->GetBlackboardComp()->SetValueAsVector(FName("selfLocation"), Player->GetActorLocation());
						HideSpots[i]->AllocatedCharacter = nullptr;
						HideSpots[i]->Availability = true;
					}
					else if (PlayerSpotDist <= 2000.f)
					{
						AIController->GetBlackboardComp()->SetValueAsVector(FName("selfLocation"), HideSpots[i]->GetActorLocation());
						AIController->GetBlackboardComp()->SetValueAsEnum(FName("AIStates"), static_cast<uint8>(EAIStates::COMBAT_MODE));
						HideSpots[i]->AllocatedCharacter = AI;
						HideSpots[i]->Availability = false;

						DrawDebugSphere(GetWorld(), HideSpots[i]->GetActorLocation(), 50.f, 4, FColor::Magenta, false, 1.0f);
					}
					break;
				}
			}
		}
		else
		{
			AIController->GetBlackboardComp()->SetValueAsVector(FName("selfLocation"), Player->GetActorLocation());
		}

	}
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