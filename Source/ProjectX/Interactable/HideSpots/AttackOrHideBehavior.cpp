// Fill out your copyright notice in the Description page of Project Settings.

#include "AttackOrHideBehavior.h"
#include "Interactable/HideSpots/HideSpotData.h"
#include "Character/AI/ProjectXAICharacter.h"
#include "Character/ProjectXPlayerCharacter.h"
#include "Runtime/Engine/Public/TimerManager.h"

// Sets default values
AAttackOrHideBehavior::AAttackOrHideBehavior()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = Scene;

}

// Called when the game starts or when spawned
void AAttackOrHideBehavior::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, this, &AAttackOrHideBehavior::Refresh, 0.5f, true, 0.5f);
}

// Called every frame
void AAttackOrHideBehavior::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FreeHideSpots = HideSpots;
	if (HideData.Num() && HideSpots.Num())
	{
		for (int32 i = 0; i < HideSpots.Num(); i++)
			for (int32 j = 0; j < HideData.Num(); j++)
				if (HideData[j]->GetHideSpotData() == HideSpots[i])
					FreeHideSpots.Remove(HideSpots[i]);
	}


	FreeCharacters = Characters;
	if (HideData.Num() && Characters.Num())
	{
		for (int32 i = 0; i < Characters.Num(); i++)
			for (int32 j = 0; j < HideData.Num(); j++)
				if (HideData[j]->GetAI() == Characters[i])
					FreeCharacters.Remove(Characters[i]);
	}


	/*
	ALLOCATE HIDESPOT TO EACH CHARACTER
	*/
	for (int32 i = 0; i < FreeCharacters.Num(); ++i)
	{
		float ShortestDist = 9999999999.f;
		AHideSpotData* hSpot = nullptr;

		for (int32 j = 0; j < FreeHideSpots.Num(); ++j)
		{
			float Dist = FVector::Dist(FreeHideSpots[j]->GetActorLocation(), FreeCharacters[i]->GetActorLocation());
			if (Dist < ShortestDist)
			{
				ShortestDist = Dist;
				hSpot = HideSpots[j];
			}
		}

		UHideData* TempDat = NewObject<UHideData>(UHideData::StaticClass());
		TempDat->SetAI(FreeCharacters[i]);
		TempDat->SetHideSpotData(hSpot);
		HideData.Add(TempDat);
		FreeCharacters.RemoveAt(i);
		FreeHideSpots.Remove(hSpot);
	}
}

void AAttackOrHideBehavior::Refresh()
{
	if (HideData.Num())
	{
		for (int32 i = 0; i < HideData.Num(); ++i)
		{
			if (HideData[i]->GetAI())
				GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, FString("AI : ").Append(HideData[i]->GetAI()->GetName()));
			if (HideData[i]->GetHideSpotData())
				GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Blue, FString("SPOT : ").Append(HideData[i]->GetHideSpotData()->GetName()));
			GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Blue, FString("______________________________________"));
		}
		
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Black, FString::FromInt(HideData.Num()));
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, FString("FreeCharcter : ").Append(FString::FromInt(FreeCharacters.Num())));
	}
}

void AAttackOrHideBehavior::Empty()
{
	HideData.Empty();
}

void AAttackOrHideBehavior::RegisterAICharacter(AProjectXAICharacter * AI)
{
	Characters.Add(AI);
}

void AAttackOrHideBehavior::RegisterHideSpot(AHideSpotData * HideSpot)
{
	HideSpots.Add(HideSpot);
}

void AAttackOrHideBehavior::UnRegisterAICharacter(AProjectXAICharacter * AI)
{
	Characters.Remove(AI);
}

void AAttackOrHideBehavior::UnRegisterHideSpot(AHideSpotData * HideSpot)
{
	HideSpots.Remove(HideSpot);
}

AHideSpotData * AAttackOrHideBehavior::GetHideSpotAssignedToCharacter(AProjectXAICharacter * Char)
{
	for (int32 i = 0; i < HideData.Num(); ++i)
		if (HideData[i]->GetAI() == Char)
			return HideData[i]->GetHideSpotData();
	return nullptr;
}

AProjectXAICharacter * AAttackOrHideBehavior::GetCharacterAssignedToHideSpot(AHideSpotData * HideSpot)
{
	for (int32 i = 0; i < HideData.Num(); ++i)
		if (HideData[i]->GetHideSpotData() == HideSpot)
			return HideData[i]->GetAI();
	return nullptr;
}

