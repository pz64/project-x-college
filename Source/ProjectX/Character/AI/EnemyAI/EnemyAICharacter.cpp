// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyAICharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Runtime/Engine/Classes/Engine/SkeletalMesh.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectX/Character/ProjectXAnimInstance.h"
#include "EnemyAIBehavior.h"
#include "Interactable/HideSpots/AttackOrHideBehavior.h"
#include "EngineUtils.h"


AEnemyAICharacter::AEnemyAICharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// IK FOOT PLACING!!!
	GATE = false;
	IKFeetInterpSpeed = 13.f;
	IKHipsInterpSpeed = 7.f;
	IKTraceDistance = 55.f;
	IKAdjustOffset = 2.f;
	IKTimeout = 0.5f;

	IKLeftSocket = "foot_lSocket";
	IKRightSocket = "foot_rSocket";
	IKCapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	PlayerType = EPlayerType::ENEMY;
}

void AEnemyAICharacter::BeginPlay()
{
	Super::BeginPlay();
	AnimInstance = Cast<UProjectXAnimInstance>(GetMesh()->GetAnimInstance());

	FindAllActors<AEnemyAIBehavior>(GetWorld(), groupbehavior);
	if (groupbehavior.Num())
		for (AEnemyAIBehavior* GroupBehaviour : groupbehavior)
			GroupBehaviour->RegisterAI(this);

}

void AEnemyAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaSeconds);
	DeltaSeconds = DeltaTime;
	EnableIK();
	IKUpdate();
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