// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interactable/InteractableInterface.h"
#include "Interactable/Weapon/WeaponInterface.h"
#include "HUD/HUDInterface.h"
#include "HUD/HealthHUD.h"
#include "ProjectXCharacter.generated.h"

UCLASS(config = Game)
class AProjectXCharacter : public ACharacter, public IInteractableInterface, public IWeaponInterface, public IHUDInterface
{
	GENERATED_BODY()

		/** Camera boom positioning the camera behind the character */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;
public:
	AProjectXCharacter();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CharacterMovement)
		float walkSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CharacterMovement)
		float runSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CharacterMovement)
		float sprintSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CharacterMovement)
		float crouchSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CharacterMovement)
		bool isCrouching;

protected:
	void ActivateIronSightCam();
	void DeactivateIronSightCam();

public:
	UFUNCTION(BlueprintCallable, Category = CharMovement)
		void Walk();
	UFUNCTION(BlueprintCallable, Category = CharMovement)
		void Run();
	UFUNCTION(BlueprintCallable, Category = CharMovement)
		void Sprint();
	UFUNCTION(BlueprintCallable, Category = CharMovement)
		void StartCrouch();
	UFUNCTION(BlueprintCallable, Category = Weapon)
		void StartFire();
	UFUNCTION(BlueprintCallable, Category = Weapon)
		void StopFire();

protected:
	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/**
	* Called via input to turn look up/down at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/////////////////////
	//				    //
	//				    //
	// IK Foot Placing	//
	//				    //
	//				    //
	/////////////////////

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = IK)
		FName IKLeftSocket;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = IK)
		FName IKRightSocket;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = IK)
		float IKTraceDistance;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = IK)
		float IKHipsInterpSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = IK)
		float IKFeetInterpSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = IK)
		float IKCapsuleHalfHeight;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = IK)
		float IKAdjustOffset;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = IK)
		float IKTimeout;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = IK)
		float DeltaSeconds;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = IK)
		class UProjectXAnimInstance* AnimInstance;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = IK)
		bool HitResult;

	bool GATE;
	void OpenGate();
	void CloseGate();

	UFUNCTION(BlueprintNativeEvent, Category = IK)
		void EnableIK();
	void EnableIK_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = IK)
		void IKResetVars();
	void IKResetVars_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = IK)
		void DisableIK();
	void DisableIK_Implementation();

	void IKUpdate();
	void IKFootTrace(const float &TraceDistance, const FName &SocketName, float &Offset, FVector &impact);
	void IKUpdateFootRotation(const FRotator &TargetValue, FRotator &RotationVar, const float &InterpSpeed);
	void IKUpdateFootOffset(const float &TargetValue, float &EffectorVar, const float &IntrpSpeed);
	void UpdateCapsuleHalfHeight(const float &HipsShift, const bool &ResetDefault);

	FRotator NormalToRotator(const FVector &Normal);
	bool IsMoving();

	/////////////////
	//				//
	//				//
	//    FPS Cam  	//
	//				//
	//				//
	/////////////////

protected:
	void SwapCameras();

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool viewThroughScope;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isFPS;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* currentCam;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* FPSCam;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* FPSArm;
public:
	FORCEINLINE class UCameraComponent* GetFPSCamera() const { return FPSCam; }
	FORCEINLINE class UCameraComponent* GetCurrentCamera() const { return currentCam; }

	/////////////////
	//				//
	//				//
	// Interactable	//
	//				//
	//				//
	/////////////////

protected:
	UFUNCTION(BlueprintCallable, Category = Weapon)
		void PerformInteraction();
protected:
	TArray<class AInteractable*> interactableList;
	class AInteractable* BestInteractable;
	float BestResult;
	void GetBestInteractable();

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Interactable)
		bool handFree = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Interactable)
		class AWeapon* HoldingWeapon;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Interactable)
		void Interact(class AInteractable *actor);
	virtual void Interact_Implementation(class AInteractable *actor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Interactable)
		void UnInteract(class AInteractable *actor);
	virtual void UnInteract_Implementation(class AInteractable *actor);

	/////////////////////
	//				    //
	//				    //
	// WeaponInterface	//
	//				    //
	//				    //
	/////////////////////

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		FName WeaponHoldSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		bool isFiring;

	void ReloadAction();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Interactable)
		void PickedUp(class AWeapon* weapon, class AProjectXCharacter* who);
	virtual void PickedUp_Implementation(class AWeapon* weapon, class AProjectXCharacter* who);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Interactable)
		void PutDown(class AWeapon* weapon);
	virtual void PutDown_Implementation(class AWeapon* weapon);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Interactable)
		void PickedUpUpdatedPhysics(class AWeapon* weapon);
	virtual void PickedUpUpdatedPhysics_Implementation(class AWeapon* weapon);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponInterface)
		bool ReloadWeapon;

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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = WeaponInterface)
		void FiringNotify(class AWeapon* weapon);
	virtual void FiringNotify_Implementation(class AWeapon* weapon);

	void FireRecoilComplete();

	/////////////////
	//				//
	//				//
	// HUDInterface	//
	//				//
	//				//
	/////////////////


public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = HUDInterface)
		void ShowWeaponPickupHUD(TSubclassOf<class UWeaponPickupHUD> HUD, const class UTexture2D* Texture, const FString& WeaponName);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = HUDInterface)
		void HideWeaponPickupHUD(TSubclassOf<class UWeaponPickupHUD> HUD);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUDInterface)
		TSubclassOf<class UWeaponPickupHUD> WeaponPickupHUD;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = HUDInterface)
		void NotifyActivateHUD();
	virtual void NotifyActivateHUD_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = HUDInterface)
		void NotifyDeactivateHUD();
	virtual void NotifyDeactivateHUD_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = HUDInterface)
		void ActivateHUD(TSubclassOf<class UMainHUD> HUD);
	virtual void ActivateHUD_Implementation(TSubclassOf<class UMainHUD> HUD);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = HUDInterface)
		void DeactivateHUD(TSubclassOf<class UMainHUD> HUD);
	virtual void DeactivateHUD_Implementation(TSubclassOf<class UMainHUD> HUD);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = HUDInterface)
		void RefreshHUD(TSubclassOf<class UMainHUD> HUD);
	virtual void RefreshHUD_Implementation(TSubclassOf<class UMainHUD> HUD);


	/////////////////////
	//				    //
	//				    //
	//     Inventory	//
	//				    //
	//				    //
	/////////////////////

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Inventory)
		class AWeapon* Inventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
		bool SwitchNow;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
		FName WeaponRestSocket;

	UFUNCTION(BlueprintCallable, Category = Inventory)
		void SwitchWeapon();

	UFUNCTION(BlueprintCallable, Category = Inventory)
		void DoSwitching();

	/////////////////////
	//				    //
	//				    //
	//     Health   	//
	//				    //
	//				    //
	/////////////////////

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
		float Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
		TSubclassOf<class UHealthHUD> HealthHUD;
	bool Revive;
	FTimerHandle Handle;

	FTimerHandle KillerHandle;

	void KillCharacter();

public:

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;


	UFUNCTION(BlueprintCallable, Category = Health)
		float GetCurrentHealth();

	UFUNCTION(BlueprintCallable, Category = Health)
		void Die();

	UFUNCTION(BlueprintCallable, Category = Health)
		bool IsDead();


	void CanRevive();

};

