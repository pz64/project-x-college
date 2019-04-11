// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AttackOrHideBehavior.generated.h"

UCLASS()
class PROJECTX_API UHideData : public UObject
{
	GENERATED_BODY()

protected:
	class AProjectXAICharacter* AI;
	class AHideSpotData* HideSpot;
public:

	void SetAI(class AProjectXAICharacter* Chara) { AI = Chara; }
	void SetHideSpotData(class AHideSpotData* LWall) { HideSpot = LWall; }

	class AHideSpotData* GetHideSpotData() { return HideSpot; }
	class AProjectXAICharacter* GetAI() { return AI; }
};

UCLASS()
class PROJECTX_API AAttackOrHideBehavior : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = GroupBehavior)
		USceneComponent* Scene;

public:
	// Sets default values for this actor's properties
	AAttackOrHideBehavior();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


protected:

	TArray<class AHideSpotData*> HideSpots;
	TArray<class AProjectXAICharacter* > Characters;

	TArray<class AHideSpotData*> FreeHideSpots;
	TArray<class AProjectXAICharacter*> FreeCharacters;

	TArray<UHideData*> HideData;

	void Refresh();
	
public:

	UFUNCTION(BlueprintCallable, Category = AttackOrHideBehavior)
		void Empty();

	UFUNCTION(BlueprintCallable, Category = AttackOrHideBehavior)
		void RegisterAICharacter(class AProjectXAICharacter* AI);

	UFUNCTION(BlueprintCallable, Category = AttackOrHideBehavior)
		void RegisterHideSpot(class AHideSpotData* HideSpot);

	UFUNCTION(BlueprintCallable, Category = AttackOrHideBehavior)
		void UnRegisterAICharacter(class AProjectXAICharacter* AI);

	UFUNCTION(BlueprintCallable, Category = AttackOrHideBehavior)
		void UnRegisterHideSpot(class AHideSpotData* HideSpot);

	UFUNCTION(BlueprintCallable, Category = AttackOrHideBehavior)
		class AHideSpotData* GetHideSpotAssignedToCharacter(class AProjectXAICharacter* Char);

	UFUNCTION(BlueprintCallable, Category = AttackOrHideBehavior)
		class AProjectXAICharacter* GetCharacterAssignedToHideSpot(class AHideSpotData* HideSpot);

};
