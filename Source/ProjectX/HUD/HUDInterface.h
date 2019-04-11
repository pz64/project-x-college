// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUDInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHUDInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTX_API IHUDInterface 
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = WeaponInterface)
		void NotifyActivateHUD();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = WeaponInterface)
		void NotifyDeactivateHUD();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = WeaponInterface)
		void ActivateHUD(TSubclassOf<class UMainHUD> HUD);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = WeaponInterface)
		void DeactivateHUD(TSubclassOf<class UMainHUD> HUD);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = WeaponInterface)
		void RefreshHUD(TSubclassOf<class UMainHUD> HUD);
};
