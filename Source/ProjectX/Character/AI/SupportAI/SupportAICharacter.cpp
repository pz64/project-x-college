// Fill out your copyright notice in the Description page of Project Settings.

#include "SupportAICharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Runtime/Engine/Classes/Engine/SkeletalMesh.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectX/Character/ProjectXAnimInstance.h"
#include "AIBehavior.h"
#include "EngineUtils.h"
#include "Interactable/HideSpots/AttackOrHideBehavior.h"

ASupportAICharacter::ASupportAICharacter()
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


	PlayerType = EPlayerType::SUPPORT;
}

void ASupportAICharacter::BeginPlay()
{
	Super::BeginPlay();
	AnimInstance = Cast<UProjectXAnimInstance>(GetMesh()->GetAnimInstance());

	FindAllActors<AAIBehavior>(GetWorld(), groupbehavior);
	if (groupbehavior.Num() > 0)
		for (AAIBehavior* GroupBehaviour : groupbehavior)
			GroupBehaviour->RegisterAI(this);
}

void ASupportAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaSeconds);
	DeltaSeconds = DeltaTime;
	EnableIK();
	IKUpdate();

}

