// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatBehavior.generated.h"

UCLASS()
class PROJECTX_API UCombatData : public UObject
{
	GENERATED_BODY()

protected:
	class AEnemyAICharacter* EnemyAI;
	class ASupportAICharacter* SupportAI;
	
public:

	void setEnemyAI(class AEnemyAICharacter* EnemyChar) { EnemyAI = EnemyChar; }
	void setSupportAI(class ASupportAICharacter* SupportChar) { SupportAI = SupportChar; }

	class AEnemyAICharacter* getEnemyAI() { return EnemyAI; }
	class ASupportAICharacter* getSupportAI() { return SupportAI; }
}; 

UCLASS()
class PROJECTX_API ACombatBehavior : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = GroupBehavior)
		USceneComponent* Scene;
	
public:	
	// Sets default values for this actor's properties
	ACombatBehavior();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


protected:
	
	TArray<class AEnemyAICharacter*> EnemyAIs;
	TArray<class ASupportAICharacter*> SupportAIs;

	TArray<class AEnemyAICharacter*> FreeEnemyAIs;
	TArray<class ASupportAICharacter*> FreeSupportAIs;

	class AProjectXPlayerCharacter* PlayerCharacter;

	TArray<UCombatData*> CombatDatas;
	
	void Logg();

public:

	UFUNCTION(BlueprintCallable, Category = CombatBehavior)
		void UnRegisterEnemyAI(class AEnemyAICharacter* EnemyAI);

	UFUNCTION(BlueprintCallable, Category = CombatBehavior)
		void UnRegisterSupportAI(class ASupportAICharacter* SupportAI);

	UFUNCTION(BlueprintCallable, Category = CombatBehavior)
		void RegisterEnemyAI(class AEnemyAICharacter* EnemyAI);

	UFUNCTION(BlueprintCallable, Category = CombatBehavior)
		void RegisterSupportAI(class ASupportAICharacter* SupportAI);

	UFUNCTION(BlueprintCallable, Category = CombatBehavior)
		void RegisterPlayer(class AProjectXPlayerCharacter* Player);

	UFUNCTION(BlueprintCallable, Category = CombatBehavior)
		class ASupportAICharacter* GetSupportAIOf(class AEnemyAICharacter* Enemy);

	UFUNCTION(BlueprintCallable, Category = CombatBehavior)
		class AEnemyAICharacter* GetEnemyAIOf(ASupportAICharacter* SupportAI);
};

template<typename T>
void FindAllActors(UWorld* World, TArray<T*>& Out);
