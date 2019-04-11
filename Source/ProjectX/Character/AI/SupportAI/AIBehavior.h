// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AI/AIStates.h"
#include "GameFramework/Actor.h"
#include "AIBehavior.generated.h" 

UCLASS()
class PROJECTX_API UAIWeaponData : public UObject
{
	GENERATED_BODY()
protected:
	class AWeapon* weapon;
	class AWeapon* Inventory;
	class ASupportAICharacter* AI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AIBehavior)
		EAIStates AIState;
public:
	FORCEINLINE EAIStates getAIState() { return AIState; };

	FORCEINLINE class AWeapon* getWeapon() { return weapon; }
	FORCEINLINE class AWeapon* getInventory() { return Inventory; }

	FORCEINLINE class ASupportAICharacter* getAI() { return AI; }

	void setAIState(EAIStates newState) { AIState = newState; }

	void setInventory(class AWeapon* weapon) { this->Inventory = weapon; }
	void setWeapon(class AWeapon* weapon) { this->weapon = weapon; }

	void setAI(class ASupportAICharacter* character) { this->AI = character; }

};

UCLASS()
class PROJECTX_API AAIBehavior : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = GroupBehavior)
		USceneComponent* Scene;

public:
	// Sets default values for this actor's properties
	AAIBehavior();

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
		void RegisterAI(class ASupportAICharacter* character);

	UFUNCTION(BlueprintCallable, Category = GroupBehavior)
		void UnRegisterAI(class ASupportAICharacter* character);


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

	TArray<class ASupportAICharacter*> Characters;
	TArray<class ASupportAICharacter*> FreeCharacters;

	uint16 WeaponCount;
	uint16 AICount;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AIBehavior)
		TArray<UAIWeaponData*> AIWeaponData;

	void Logg();

public:
	UFUNCTION(BlueprintCallable, Category = AIBehavior)
		class AWeapon* GetWeaponAssignedToCharacter(class ASupportAICharacter* character);
	UFUNCTION(BlueprintCallable, Category = AIBehavior)
		class ASupportAICharacter* GetChacterAssignedToWeapon(class AWeapon* weapon);
	UFUNCTION(BlueprintCallable, Category = AIBehavior)
		class AWeapon* GetInventoryAssignedToCharacter(class ASupportAICharacter* character);

};

inline bool CharacterPresentIn(TArray<class UAIWeaponData*> Arr, class ASupportAICharacter* Val);
inline bool WeaponPresentIn(TArray<class UAIWeaponData*> Arr, class AWeapon* Val);
inline bool InventoryPresentIn(TArray<class UAIWeaponData*> Arr, class AWeapon* Val);