// Fill out your copyright notice in the Description page of Project Settings.

#include "FindAttackSpot.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Character/AI/ProjectXAIController.h"
#include "Character/AI/ProjectXAICharacter.h"
#include "Interactable/HideSpots/HideSpotData.h"
#include "Character/AI/AIStates.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Runtime/AIModule/Classes/EnvironmentQuery/EnvQuery.h"
#include "Runtime/AIModule/Classes/EnvironmentQuery/EnvQueryTypes.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"
#include "EngineUtils.h"


UFindAttackSpot::UFindAttackSpot()
{
	static ConstructorHelpers::FObjectFinder<UEnvQuery> EQS_Query(TEXT("EnvQuery'/Game/Game/Characters/AI/AI_Support/EnvQueries/EQ_GetAvailableHideSpots'"));
	Query = EQS_Query.Object;

	if (Query != nullptr)
	{
		QueryRequest = FEnvQueryRequest(Query, this);
	}
}

EBTNodeResult::Type UFindAttackSpot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AIController = Cast<AProjectXAIController>(OwnerComp.GetOwner());
	AI = AIController ? Cast<AProjectXAICharacter>(AIController->GetPawn()) : nullptr;

	QueryRequest.Execute(EEnvQueryRunMode::AllMatching, this, &UFindAttackSpot::QueryFinished);


	return EBTNodeResult::Succeeded;
}


void UFindAttackSpot::QueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
	UE_LOG(LogTemp, Warning, TEXT("C++ EQS Completed"));

	TArray<AActor*> AllDetectedActors;
	Result->GetAllAsActors(AllDetectedActors);

	UE_LOG(LogTemp, Warning, TEXT("EQS %i"), AllDetectedActors.Num());


	if (AI)
	{
		if (AllDetectedActors.Num())
		{

			float shortestDist = 999999999.0f;
			AHideSpotData* tempData = nullptr;

			for (int32 i = 0; i < AllDetectedActors.Num(); ++i)
			{
				if (Cast<AHideSpotData>(AllDetectedActors[i])->AllocatedCharacter == AI)
					break;

				DrawDebugSphere(GetWorld(), AllDetectedActors[i]->GetActorLocation(), 100.f, 8, FColor::Blue, false, 1.0f);

				float dist = FVector::Dist(AI->GetActorLocation(), Cast<AHideSpotData>(AllDetectedActors[i])->GetActorLocation());
				if (shortestDist > dist && Cast<AHideSpotData>(AllDetectedActors[i])->Availability && !Cast<AHideSpotData>(AllDetectedActors[i])->AllocatedCharacter)
				{
					shortestDist = dist;
					tempData = Cast<AHideSpotData>(AllDetectedActors[i]);
				}
			}

			if (tempData)
			{
				tempData->AllocatedCharacter = AI;
				tempData->Availability = false;
			}
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