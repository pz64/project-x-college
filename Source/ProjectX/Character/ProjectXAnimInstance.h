// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ProjectXAnimInstance.generated.h"

/**
*
*/
UCLASS()
class PROJECTX_API UProjectXAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = IK)
		FRotator LeftFootRotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = IK)
		FRotator RightFootRotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = IK)
		FVector LeftJoinLoc;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = IK)
		FVector RightJoinLoc;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = IK)
		float LeftEffectorLoc;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = IK)
		float RightEffectorLoc;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = IK)
		float HipsOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AIM)
		float AimYaw;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AIM)
		float AimPitch;

	UProjectXAnimInstance();


};
