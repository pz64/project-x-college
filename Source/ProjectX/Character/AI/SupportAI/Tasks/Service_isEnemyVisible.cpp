// Fill out your copyright notice in the Description page of Project Settings.

#include "Service_isEnemyVisible.h"
#include "Character/AI/SupportAI/SupportAICharacter.h"
#include "Character/AI/CombatBehavior.h"
#include "Character/AI/SupportAI/SupportAIController.h"
#include "Character/AI/EnemyAI/EnemyAICharacter.h"
#include "Camera/CameraComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Interactable/Weapon/Weapon.h"
#include "Components/CapsuleComponent.h"
#include "EngineUtils.h"


UService_isEnemyVisible::UService_isEnemyVisible()
{
	bCreateNodeInstance = true;
}

void UService_isEnemyVisible::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ASupportAIController* AIController = Cast<ASupportAIController>(OwnerComp.GetOwner());
	ASupportAICharacter* AI = AIController ? Cast<ASupportAICharacter>(AIController->GetPawn()) : nullptr;

	if (AI)
	{
		AEnemyAICharacter* EnemyAI = Cast<AEnemyAICharacter> (AIController->GetBlackboardComp()->GetValueAsObject(FName("Enemy")));

		if (EnemyAI && AI->HoldingWeapon)
		{


			FVector src = AI->HoldingWeapon->meshComponent->GetSocketLocation("Muzzle");
			FVector dst = FRotationMatrix(AI->GetCurrentCamera()->GetComponentRotation()).GetScaledAxis(EAxis::X) * 999999999.f + src;

			FHitResult HitResult(ForceInit);

			FCollisionQueryParams traceqparam;
			traceqparam.AddIgnoredComponent(AI->GetCapsuleComponent());
			traceqparam.AddIgnoredActor(AI);
			traceqparam.AddIgnoredActor(AI->HoldingWeapon);

			

			bool successfulHit = GetWorld()->LineTraceSingleByChannel(HitResult, src, dst, ECollisionChannel::ECC_Camera, traceqparam);


			if (successfulHit && HitResult.GetActor() && HitResult.GetActor()->IsA<AEnemyAICharacter>())
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