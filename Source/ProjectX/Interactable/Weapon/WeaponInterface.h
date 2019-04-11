// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWeaponInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTX_API IWeaponInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = WeaponInterface)
		void BeginFire(class AWeapon* weapon);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = WeaponInterface)
		void EndFire(class AWeapon* weapon);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = WeaponInterface)
		void FiringNotify(class AWeapon* weapon);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = WeaponInterface)
		void Reload(class AWeapon* weapon);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = WeaponInterface)
		void ReloadComplete();
	

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = WeaponInterface)
		void PickedUp(class AWeapon* weapon, class AProjectXCharacter* who);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = WeaponInterface)
		void PickedUpUpdatedPhysics(class AWeapon* weapon);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = WeaponInterface)
		void PutDown(class AWeapon* weapon);
};
