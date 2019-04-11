// Fill out your copyright notice in the Description page of Project Settings.

#include "Service_isSupportAIVisible.h"
#include "Character/AI/EnemyAI/EnemyAICharacter.h"
#include "Character/AI/CombatBehavior.h"
#include "Character/AI/EnemyAI/EnemyAIController.h"
#include "Character/AI/SupportAI/SupportAICharacter.h"
#include "Character/ProjectXPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Interactable/Weapon/Weapon.h"
#include "Components/CapsuleComponent.h"
#include "Character/ProjectXPlayerCharacter.h"
#include "EngineUtils.h"
#include "Character/AI/AIStates.h"

UService_isSupportAIVisible::UService_isSupportAIVisible()
{
	bCreateNodeInstance = true;
}

void UService_isSupportAIVisible::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AEnemyAIController* AIController = Cast<AEnemyAIController>(OwnerComp.GetOwner());
	AEnemyAICharacter* AI = AIController ? Cast<AEnemyAICharacter>(AIController->GetPawn()) : nullptr;
	AProjectXPlayerCharacter* Player = Cast<AProjectXPlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

	if (AI)
	{
		ASupportAICharacter* SupportAI = Cast<ASupportAICharacter>(AIController->GetBlackboardComp()->GetValueAsObject(FName("Enemy")));

		FVector src = AI->HoldingWeapon->meshComponent->GetSocketLocation("Muzzle");
		FVector dst = FRotationMatrix(AI->GetCurrentCamera()->GetComponentRotation()).GetScaledAxis(EAxis::X) * 999999999.f + src;

		FHitResult HitResult(ForceInit);

		FCollisionQueryParams traceqparam;
		traceqparam.AddIgnoredComponent(AI->GetCapsuleComponent());
		traceqparam.AddIgnoredActor(AI);
		traceqparam.AddIgnoredActor(AI->HoldingWeapon);

		bool successfulHit = GetWorld()->LineTraceSingleByChannel(HitResult, src, dst, ECollisionChannel::ECC_Camera, traceqparam);

		if (SupportAI && AI->HoldingWeapon)
		{
			if (successfulHit && HitResult.GetActor() && HitResult.GetActor()->IsA<ASupportAICharacter>())
			{
				AIController->GetBlackboardComp()->SetValueAsBool(FName("canShoot"), true);
				AIController->GetBlackboardComp()->SetValueAsObject(FName("Enemy"), HitResult.GetActor());
			}
			else
			{
				AIController->GetBlackboardComp()->SetValueAsBool(FName("canShoot"), false);
				AIController->GetBlackboardComp()->SetValueAsObject(FName("Enemy"), nullptr);
			}

		}
		else if(Player)
		{
			if (successfulHit && HitResult.GetActor() && HitResult.GetActor()->IsA<AProjectXPlayerCharacter>())
			{
				AIController->GetBlackboardComp()->SetValueAsBool(FName("canShoot"), true);
				AIController->GetBlackboardComp()->SetValueAsObject(FName("Enemy"), HitResult.GetActor());
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