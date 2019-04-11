// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable/Interactable.h"
#include "Interactable/Weapon/WeaponInterface.h"
#include "HUD/HUDInterface.h"
#include "Weapon.generated.h"


/**
 *
 */
UCLASS()
class PROJECTX_API AWeapon : public AInteractable, public IWeaponInterface, public IHUDInterface
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	AWeapon();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Weapon Property

protected:
	FTimerHandle Handle;

	UFUNCTION(BlueprintCallable, Category = Weapon)
		void FireOnce();
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Weapon)
		class UParticleSystemComponent* MuzzleFlash;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		class UParticleSystem* FlashParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		bool isFiring;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		bool canFire;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Weapon)
		TSubclassOf< class ABulletImpact> ImpactForce;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		class AProjectXCharacter* Shooter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		FName MuzzleSocket;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		float RecoilTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		float RecoilPower; // Camera shake value
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		int32 MagazineSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		int32 MagazineSizeMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		int32 MagazineCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		float BulletVelocity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		TSubclassOf<class UCrosshairHUD> CrosshairHUD;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		TSubclassOf<class UWeaponCounterHUD> BulletCounterHUD;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		class UCameraComponent* ScopeCam;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		bool ScopeHasFocusCapability;

	void SetShooter(class AProjectXCharacter* Shooter);

	FHitResult HitResult;
	void SpawnImapact();


	/////////////////////
	//				    //
	//				    //
	// Texture and name	//
	//				    //
	//				    //
	/////////////////////

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Name)
		class UTexture2D* Texture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Name)
		FString WeaponName;

	/////////////////////
	//				    //
	//				    //
	// WeaponInterface	//
	//				    //
	//				    //
	/////////////////////

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = WeaponInterface)
		void FiringNotify(class AWeapon* weapon);
	virtual void FiringNotify_Implementation(class AWeapon* weapon);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = WeaponInterface)
		void BeginFire(class AWeapon* weapon);
	virtual void BeginFire_Implementation(class AWeapon* weapon);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = WeaponInterface)
		void EndFire(class AWeapon* weapon);
	virtual void EndFire_Implementation(class AWeapon* weapon);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = WeaponInterface)
		void Reload(class AWeapon* weapon);
	virtual void Reload_Implementation(class AWeapon* weapon);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = WeaponInterface)
		void ReloadComplete();
	virtual void ReloadComplete_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Interactable)
		void PickedUp(class AWeapon* weapon, class AProjectXCharacter* who);
	virtual void PickedUp_Implementation(class AWeapon* weapon, class AProjectXCharacter* who);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Interactable)
		void PutDown(class AWeapon* weapon);
	virtual void PutDown_Implementation(class AWeapon* weapon);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Interactable)
		void PickedUpUpdatedPhysics(class AWeapon* weapon);
	virtual void PickedUpUpdatedPhysics_Implementation(class AWeapon* weapon);

	/////////////////
	//				//
	//				//
	// HUDInterface	//
	//				//
	//				//
	/////////////////

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = WeaponInterface)
		void NotifyActivateHUD();
	virtual void NotifyActivateHUD_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = WeaponInterface)
		void NotifyDeactivateHUD();
	virtual void NotifyDeactivateHUD_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = HUDInterface)
		void ActivateHUD(TSubclassOf<class UMainHUD> HUD);
	virtual void ActivateHUD_Implementation(TSubclassOf<class UMainHUD> HUD);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = HUDInterface)
		void DeactivateHUD(TSubclassOf<class UMainHUD> HUD);
	virtual void DeactivateHUD_Implementation(TSubclassOf<class UMainHUD> HUD);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = WeaponInterface)
		void RefreshHUD(TSubclassOf<class UMainHUD> HUD);
	virtual void RefreshHUD_Implementation(TSubclassOf<class UMainHUD> HUD);


	///////////////////////////
	//				          //
	//				          //
	//      GroupBehavior     //
	//				          //
	//				          //
	///////////////////////////
protected:
	TArray<class AAIBehavior*> groupbehavior;
	TArray<class AEnemyAIBehavior*> enemyGroupbehavior;

};

template<typename T>
void FindAllActors(UWorld* World, TArray<T*>& Out);
