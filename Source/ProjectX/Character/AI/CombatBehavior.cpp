// Fill out your copyright notice in the Description page of Project Settings.

#include "CombatBehavior.h"
#include "Character/ProjectXPlayerCharacter.h"
#include "Character/AI/EnemyAI/EnemyAICharacter.h"
#include "Character/AI/SupportAI/SupportAICharacter.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "EngineUtils.h"

// Sets default values
ACombatBehavior::ACombatBehavior()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = Scene;
}

// Called when the game starts or when spawned
void ACombatBehavior::BeginPlay()
{
	Super::BeginPlay();

	TArray<AProjectXPlayerCharacter*> PlayerChar;
	FindAllActors<AProjectXPlayerCharacter>(GetWorld(), PlayerChar);
	RegisterPlayer(PlayerChar[0]);

//	FTimerHandle Handle;
//	GetWorldTimerManager().SetTimer(Handle, this, &ACombatBehavior::Logg, 0.5f, true, 0.5);
}

// Called every frame
void ACombatBehavior::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (SupportAIs.Num() && EnemyAIs.Num())
	{
		//CHECK IF AIs ARE DEAD.
		for (int32 i = 0; (i < SupportAIs.Num()); ++i)
		{
			if (!IsValid(SupportAIs[i]))
			{
				SupportAIs.RemoveAt(i);
				break;
			}
		}
		for (int32 i = 0; (i < EnemyAIs.Num()); ++i)
		{
			if (!IsValid(EnemyAIs[i]))
			{
				EnemyAIs.RemoveAt(i);
			}
		}

		/*
		UPDATE THE LIST OF FREE SUPPORT AIs
		*/
		FreeSupportAIs = SupportAIs;
		if (SupportAIs.Num() && CombatDatas.Num())
		{
			for (int32 i = 0; i < SupportAIs.Num(); ++i)
				for (int32 j = 0; j < CombatDatas.Num(); ++j)
					if (SupportAIs[i] == CombatDatas[j]->getSupportAI())
						FreeSupportAIs.Remove(SupportAIs[i]);
		}

		/*
		UPDATE THE LIST OF FREE ENEMY AIs
		*/
		FreeEnemyAIs = EnemyAIs;
		if (EnemyAIs.Num() && CombatDatas.Num())
		{
			for (int32 i = 0; i < EnemyAIs.Num(); ++i)
				for (int32 j = 0; j < CombatDatas.Num(); ++j)
					if (EnemyAIs[i] == CombatDatas[j]->getEnemyAI())
						FreeEnemyAIs.Remove(EnemyAIs[i]);
		}


		/*
		ADD COMBAT-DATA INFOS
		*/
		if (FreeSupportAIs.Num() && FreeEnemyAIs.Num())
		{
			if (FreeSupportAIs.Num() < FreeEnemyAIs.Num())
			{
				for (int32 i = 0; i < FreeSupportAIs.Num(); ++i)
				{
					float ShortestDist = 9999999999.f;
					AEnemyAICharacter* TempEnemy = FreeEnemyAIs[0];

					for (int32 j = 0; j < FreeEnemyAIs.Num(); ++j)
					{
						float Distance = FVector::Dist(FreeSupportAIs[i]->GetActorLocation(), FreeEnemyAIs[j]->GetActorLocation());

						if (Distance < ShortestDist)
						{
							ShortestDist = Distance;
							TempEnemy = FreeEnemyAIs[j];
						}
					}
					UCombatData* CombatData = NewObject<UCombatData>(UCombatData::StaticClass());
					CombatData->setSupportAI(FreeSupportAIs[i]);
					CombatData->setEnemyAI(TempEnemy);
					CombatDatas.Add(CombatData);
					FreeEnemyAIs.Remove(TempEnemy);
				}
			}
			else
			{
				for (int32 i = 0; i < FreeEnemyAIs.Num(); ++i)
				{

					float ShortestDist = 9999999999.f;
					ASupportAICharacter* TempSupport = FreeSupportAIs[0];

					for (int32 j = 0; j < FreeSupportAIs.Num(); ++j)
					{
						float Distance = FVector::Dist(FreeEnemyAIs[i]->GetActorLocation(), FreeSupportAIs[j]->GetActorLocation());

						if (Distance < ShortestDist)
						{
							ShortestDist = Distance;
							TempSupport = FreeSupportAIs[j];
						}
					}
					UCombatData* CombatData = NewObject<UCombatData>(UCombatData::StaticClass());
					CombatData->setSupportAI(TempSupport);
					CombatData->setEnemyAI(EnemyAIs[i]);
					CombatDatas.Add(CombatData);
					FreeSupportAIs.Remove(TempSupport);
				}
			}
		}
	}
	else
		Destroy();
}

void ACombatBehavior::Logg()
{
	GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, FString("CombatBeavior "));
	if (CombatDatas.Num())
	{
		/*	for (int32 i = 0; i < CombatDatas.Num(); ++i)
			{
				if (CombatDatas[i]->getSupportAI())
					GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Blue, FString("SUPPORT : ").Append(CombatDatas[i]->getSupportAI()->GetName()));
				if (CombatDatas[i]->getEnemyAI())
					GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, FString("ENEMY : ").Append(CombatDatas[i]->getEnemyAI()->GetName()));
			}
		*/
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, FString("SUPPORT-COUNT : ").Append(FString::FromInt(SupportAIs.Num())));
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, FString("ENEMY-COUNT : ").Append(FString::FromInt(EnemyAIs.Num())));
	}

}

void ACombatBehavior::UnRegisterEnemyAI(AEnemyAICharacter * EnemyAI)
{
	EnemyAIs.Remove(EnemyAI);
}

void ACombatBehavior::UnRegisterSupportAI(ASupportAICharacter * SupportAI)
{
	SupportAIs.Remove(SupportAI);
}

void ACombatBehavior::RegisterEnemyAI(AEnemyAICharacter * EnemyAI)
{
	EnemyAIs.Add(EnemyAI);
}

void ACombatBehavior::RegisterSupportAI(ASupportAICharacter * SupportAI)
{
	SupportAIs.Add(SupportAI);
}

void ACombatBehavior::RegisterPlayer(AProjectXPlayerCharacter * Player)
{
	PlayerCharacter = Player;
}

ASupportAICharacter * ACombatBehavior::GetSupportAIOf(AEnemyAICharacter * Enemy)
{
	for (int32 i = 0; i < CombatDatas.Num(); ++i)
		if (CombatDatas[i]->getEnemyAI() == Enemy)
			return CombatDatas[i]->getSupportAI();
	return nullptr;
}

AEnemyAICharacter * ACombatBehavior::GetEnemyAIOf(ASupportAICharacter * SupportAI)
{
	for (int32 i = 0; i < CombatDatas.Num(); ++i)
		if (CombatDatas[i]->getSupportAI() == SupportAI)
			return CombatDatas[i]->getEnemyAI();
	return nullptr;
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



