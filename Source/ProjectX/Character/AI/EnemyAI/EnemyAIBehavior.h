// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include  "Character/AI/AIStates.h"
#include "EnemyAIBehavior.generated.h"


UCLASS()
class PROJECTX_API UEnemyAIWeaponData : public UObject
{
	GENERATED_BODY()
protected:
	class AWeapon* weapon;
	class AWeapon* Inventory;
	class AEnemyAICharacter* AI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AIBehavior)
		 EAIStates AIState;
public:
	FORCEINLINE EAIStates getAIState() { return AIState; };

	FORCEINLINE class AWeapon* getWeapon() { return weapon; }
	FORCEINLINE class AWeapon* getInventory() { return Inventory; }

	FORCEINLINE class AEnemyAICharacter* getAI() { return AI; }

	void setAIState(EAIStates newState) { AIState = newState; }

	void setInventory(class AWeapon* weapon) { this->Inventory = weapon; }
	void setWeapon(class AWeapon* weapon) { this->weapon = weapon; }

	void setAI(class AEnemyAICharacter* character) { this->AI = character; }

};


UCLASS()
class PROJECTX_API AEnemyAIBehavior : public AActor
{
	GENERATED_BODY()


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = GroupBehavior)
		USceneComponent* Scene;

public:
	// Sets default values for this actor's properties
	AEnemyAIBehavior();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	///////////////////////////
	//				          //
	//				          //
	// GroupBehaviorInterface //
	//				          //
	//				          //
	///////////////////////////

public:

	UFUNCTION(BlueprintCallable, Category = GroupBehavior)
		void RegisterWeapon(class AWeapon* weapon);

	UFUNCTION(BlueprintCallable, Category = GroupBehavior)
		void UnRegisterWeapon(class AWeapon* weapon);

	UFUNCTION(BlueprintCallable, Category = GroupBehavior)
		void RegisterAI(class AEnemyAICharacter* character);

	UFUNCTION(BlueprintCallable, Category = GroupBehavior)
		void UnRegisterAI(class AEnemyAICharacter* character);


	///////////////////////////
	//				          //
	//				          //
	//      GroupBehavior     //
	//				          //
	//				          //
	///////////////////////////
protected:

	TArray<class AWeapon*> Weapons;
	TArray<class AWeapon*> FreeWeapons;

	TArray<class AEnemyAICharacter*> Characters;
	TArray<class AEnemyAICharacter*> FreeCharacters;

	uint16 WeaponCount;
	uint16 AICount;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AIBehavior)
		TArray<UEnemyAIWeaponData*> AIWeaponData;

	void UpdateStrategy();

public:
	UFUNCTION(BlueprintCallable, Category = AIBehavior)
		class AWeapon* GetWeaponAssignedToCharacter(class AEnemyAICharacter* character);
	UFUNCTION(BlueprintCallable, Category = AIBehavior)
		class AEnemyAICharacter* GetChacterAssignedToWeapon(class AWeapon* weapon);
	UFUNCTION(BlueprintCallable, Category = AIBehavior)
		class AWeapon* GetInventoryAssignedToCharacter(class AEnemyAICharacter* character);

};

inline bool CharacterPresentIn(TArray<class UEnemyAIWeaponData*> Arr, class AEnemyAICharacter* Val);
inline bool WeaponPresentIn(TArray<class UEnemyAIWeaponData*> Arr, class AWeapon* Val);
inline bool InventoryPresentIn(TArray<class UEnemyAIWeaponData*> Arr, class AWeapon* Val);