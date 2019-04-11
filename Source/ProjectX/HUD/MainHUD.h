// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDInterface.h"
#include "MainHUD.generated.h"


/**
 * 
 */
UCLASS()
class PROJECTX_API UMainHUD : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UMG)
		float CrosshairWideX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UMG)
		float CrosshairWideY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UMG)
		int32 MagSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UMG)
		int32 MagSizeMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UMG)
		int32 MagCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UMG)
		float Health;
	
};
