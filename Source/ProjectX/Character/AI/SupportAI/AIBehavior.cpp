// Fill out your copyright notice in the Description page of Project Settings.

#include "AIBehavior.h"
#include "SupportAICharacter.h"
#include "Interactable/Weapon/Weapon.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Character/AI/EnemyAI/EnemyAIBehavior.h"

// Sets default values
AAIBehavior::AAIBehavior()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = Scene;
}

// Called when the game starts or when spawned
void AAIBehavior::BeginPlay()
{
	Super::BeginPlay();

	//	FTimerHandle Handle;
	//	GetWorldTimerManager().SetTimer(Handle, this, &AAIBehavior::UpdateStrategy, 0.5f, true, 0.5);
}

// Called every frame
void AAIBehavior::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Characters.Num())
	{
		//CHECK IF AIs ARE DEAD.

		for (int32 i = 0; (i < Characters.Num()); ++i)
		{
			if (!IsValid(Characters[i]))
			{
				if (CharacterPresentIn(AIWeaponData, Characters[i]))
				{
					for (int j = 0; j < AIWeaponData.Num(); j++)
						if (AIWeaponData[j]->getAI() == Characters[i])
							AIWeaponData.RemoveAt(j);
				}
				Characters.RemoveAt(i);

			}
		}

		/*
		UPDATE THE LIST OF FREE WEAPONS
		*/
		FreeWeapons = Weapons;
		if (AIWeaponData.Num())
		{
			for (int32 i = 0; i < Weapons.Num(); ++i)
			{
				if (InventoryPresentIn(AIWeaponData, Weapons[i]))
					FreeWeapons.Remove(Weapons[i]);
				if (WeaponPresentIn(AIWeaponData, Weapons[i]))
					FreeWeapons.Remove(Weapons[i]);
				if (Weapons[i]->Shooter)
					FreeWeapons.Remove(Weapons[i]);
			}
		}
		/*
		UPDATE THE LIST OF AI CHARACTERS UNASSIGNED
		*/
		FreeCharacters = Characters;
		if (AIWeaponData.Num())
		{
			for (int32 i = 0; i < Characters.Num(); ++i)
				if (CharacterPresentIn(AIWeaponData, Characters[i]))
					FreeCharacters.Remove(Characters[i]);
		}

		AICount = FreeCharacters.Num();
		WeaponCount = FreeWeapons.Num();

		if (AICount && WeaponCount)
		{
			for (int32 i = 0; i < (AICount < WeaponCount ? AICount : WeaponCount); ++i)
			{

				float TempDistance = 9999999999.f;
				AWeapon* TempWeapon = WeaponCount ? FreeWeapons[0] : nullptr;

				/*
				GET THE BEST WEAPON FOR EACH CHARACTER
				*/
				for (int32 j = 0; j < WeaponCount; ++j)
				{
					float Distance = FVector::Dist(FreeWeapons[j]->GetActorLocation(), FreeCharacters[i]->GetActorLocation());

					if (Distance < TempDistance)
					{
						TempDistance = Distance;
						TempWeapon = FreeWeapons[j];
					}
				}

				/*
				STORE AI-WEAPON DATA
				*/
				UAIWeaponData* Data = NewObject<UAIWeaponData>(UAIWeaponData::StaticClass());
				Data->setAI(FreeCharacters[i]);
				Data->setWeapon(TempWeapon);
				AIWeaponData.Add(Data);
				FreeWeapons.Remove(TempWeapon);
				WeaponCount = FreeWeapons.Num();
			}
		}

		if (WeaponCount && !AICount)
		{
			int32 CharCount = Characters.Num();
			for (int32 i = 0; i < CharCount; ++i)
			{
				float TempDistance = 9999999999.f;
				AWeapon* TempInventory = WeaponCount ? FreeWeapons[0] : nullptr;

				/*
				GET THE BEST WEAPON FOR EACH CHARACTER
				*/
				for (int32 j = 0; j < WeaponCount; ++j)
				{
					float Distance = FVector::Dist(FreeWeapons[j]->GetActorLocation(), Characters[i]->GetActorLocation());

					if (Distance < TempDistance)
					{
						TempDistance = Distance;
						TempInventory = FreeWeapons[j];
					}
				}

				/*
				STORE AI-WEAPON DATA
				*/
				for (int32 j = 0; j < AIWeaponData.Num(); ++j)
				{
					if (AIWeaponData[j]->getAI()->GetName() == Characters[i]->GetName() && !AIWeaponData[j]->getInventory())
					{
						AIWeaponData[j]->setInventory(TempInventory);
						AIWeaponData[j]->setAIState(EAIStates::ARMED);
						FreeWeapons.Remove(TempInventory);
						WeaponCount = FreeWeapons.Num();
						break;
					}
				}
			}
		}
		/*
		REMOVE WEAPON FROM AI-WEAPON DATA IF SHOOTER IS DIFFERENT
		*/
		if (AIWeaponData.Num())
		{
			for (int32 i = 0; i < AIWeaponData.Num(); ++i)
			{
				if (AIWeaponData[i]->getWeapon() && AIWeaponData[i]->getWeapon()->Shooter && AIWeaponData[i]->getWeapon()->Shooter != AIWeaponData[i]->getAI())
					AIWeaponData[i]->setWeapon(nullptr);
				if (AIWeaponData[i]->getInventory() && AIWeaponData[i]->getInventory()->Shooter && AIWeaponData[i]->getInventory()->Shooter != AIWeaponData[i]->getAI())
					AIWeaponData[i]->setInventory(nullptr);
			}
		}
		/*
		IF WEAPON IS NULL, SET IT (SIDE EFFECT OF ABOVE ACTION)
		*/
		if (WeaponCount && AIWeaponData.Num())
		{
			for (int32 i = 0; i < AIWeaponData.Num(); ++i)
			{
				if (!AIWeaponData[i]->getWeapon())
				{
					float TempDistance = 9999999999.f;
					AWeapon* TempWeapon = WeaponCount ? FreeWeapons[0] : nullptr;

					for (int32 j = 0; j < WeaponCount; ++j)
					{
						float Distance = FVector::Dist(FreeWeapons[j]->GetActorLocation(), AIWeaponData[i]->getAI()->GetActorLocation());

						if (Distance < TempDistance)
						{
							TempDistance = Distance;
							TempWeapon = FreeWeapons[j];
						}
					}
					AIWeaponData[i]->setWeapon(TempWeapon);
				}
			}
		}
	}
	else if (AIWeaponData.Num())
		AIWeaponData.Empty();

}

bool CharacterPresentIn(TArray<UAIWeaponData*> Arr, ASupportAICharacter* Val)
{
	for (int32 i = 0; i < Arr.Num(); ++i)
		if (Arr[i]->getAI() == Val)
			return true;
	return false;
}

bool WeaponPresentIn(TArray<UAIWeaponData*> Arr, AWeapon* Val)
{
	for (int32 i = 0; i < Arr.Num(); ++i)
		if (Arr[i]->getWeapon() == Val)
			return true;
	return false;
}

inline bool InventoryPresentIn(TArray<class UAIWeaponData*> Arr, AWeapon * Val)
{
	for (int32 i = 0; i < Arr.Num(); ++i)
		if (Arr[i]->getInventory() == Val)
			return true;
	return false;
}


void AAIBehavior::Logg()
{
	for (int32 i = 0; i < AIWeaponData.Num(); ++i)
	{
		if (AIWeaponData[i]->getAI())
			GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, FString("AI : ").Append(AIWeaponData[i]->getAI()->GetName()));
		if (AIWeaponData[i]->getWeapon())
			GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Blue, FString("WEAPON : ").Append(AIWeaponData[i]->getWeapon()->GetName()));
		if (AIWeaponData[i]->getInventory())
			GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Blue, FString("INVENTORY : ").Append(AIWeaponData[i]->getInventory()->GetName()));
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, "------------------------");
	}

	GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, FString("AI-WEAPON COUNT : ").Append(FString::FromInt(AIWeaponData.Num())));
	GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, FString("AI COUNT : ").Append(FString::FromInt(AICount)));
	GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, FString("WEAPON COUNT : ").Append(FString::FromInt(WeaponCount)));

}

AWeapon * AAIBehavior::GetWeaponAssignedToCharacter(ASupportAICharacter * character)
{
	for (int32 i = 0; i < AIWeaponData.Num(); ++i)
		if (AIWeaponData[i]->getAI() == character)
			return AIWeaponData[i]->getWeapon();
	return nullptr;
}

ASupportAICharacter * AAIBehavior::GetChacterAssignedToWeapon(AWeapon * weapon)
{
	for (int32 i = 0; i < AIWeaponData.Num(); ++i)
		if (AIWeaponData[i]->getWeapon() == weapon)
			return AIWeaponData[i]->getAI();
	return nullptr;
}

AWeapon * AAIBehavior::GetInventoryAssignedToCharacter(ASupportAICharacter * character)
{
	for (int32 i = 0; i < AIWeaponData.Num(); ++i)
		if (AIWeaponData[i]->getAI() == character)
			return AIWeaponData[i]->getInventory();
	return nullptr;
}

void AAIBehavior::RegisterWeapon(class AWeapon * weapon)
{
	Weapons.Add(weapon);
}


void AAIBehavior::UnRegisterWeapon(class AWeapon * weapon)
{
	Weapons.Remove(weapon);
}

void AAIBehavior::RegisterAI(class ASupportAICharacter * charater)
{
	Characters.Add(charater);
}

void AAIBehavior::UnRegisterAI(class ASupportAICharacter * charater)
{
	Characters.Remove(charater);
}



