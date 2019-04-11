// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "Jeep.generated.h"

/**
 *
 */
UCLASS()
class PROJECTX_API AJeep : public AWheeledVehicle
{
	GENERATED_BODY()

protected:

	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* SpringArm;

	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* Camera;

public:

	AJeep();

	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	virtual void Tick(float Delta) override;
	virtual void BeginPlay() override;

	void MoveForward(float Val);
	void UpdatePhysicsMaterial();
	void MoveRight(float Val);
	void OnHandbrakePressed();
	void OnHandbrakeReleased();

public:
	
	FORCEINLINE class USpringArmComponent* GetSpringArm() const { return SpringArm; }
	
	FORCEINLINE class UCameraComponent* GetCamera() const { return Camera; }

};
