// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ProjectXCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "ProjectXAnimInstance.h"
#include "Interactable/Interactable.h"
#include "Interactable/Weapon/Weapon.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "HUD/MainHUD.h"
#include "HUD/WeaponPickupHUD.h"
#include "Runtime/Engine/Classes/Engine/Texture2D.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Character/ProjectXPlayerCharacter.h"
#include "Character/AI/ProjectXAICharacter.h"

//////////////////////////////////////////////////////////////////////////
// AProjectXCharacter

AProjectXCharacter::AProjectXCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 10.0f;
	CameraBoom->CameraLagMaxDistance = 60.f;
	CameraBoom->CameraLagMaxTimeStep = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	walkSpeed = 300;
	runSpeed = 600;
	sprintSpeed = 900;
	crouchSpeed = 150;


	// IK FOOT PLACING!!!

	GATE = false;
	IKFeetInterpSpeed = 13.f;
	IKHipsInterpSpeed = 7.f;
	IKTraceDistance = 55.f;
	IKAdjustOffset = 2.f;
	IKTimeout = 0.5f;

	IKLeftSocket = "foot_lSocket";
	IKRightSocket = "foot_rSocket";
	IKCapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	// FPS CAMERA

	FPSArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("FPSArm"));
	FPSArm->SetupAttachment(GetMesh(), "head");
	FPSArm->TargetArmLength = 0.f;
	FPSArm->bUsePawnControlRotation = true;

	FPSArm->bEnableCameraLag = true;
	FPSArm->CameraLagSpeed = 10.f;
	FPSArm->CameraLagMaxDistance = 3.f;
	FPSArm->CameraLagMaxTimeStep = true;
	FPSArm->bEnableCameraRotationLag = true;
	FPSArm->CameraRotationLagSpeed = 15.f;

	FPSCam = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCamera"));
	FPSCam->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));
	FPSCam->AddLocalRotation(FRotator(0.0f, 0.0f, 0.0f));
	FPSCam->AddLocalRotation(FRotator(0.0f, 90.0f, -90.0f));
	FPSCam->AddLocalOffset(FVector(3.0f, 0.0f, 0.0f));
	FPSCam->bUsePawnControlRotation = true;
	FPSCam->SetupAttachment(FPSArm, USpringArmComponent::SocketName);

	GetMesh()->AddLocalRotation(FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->AddLocalOffset(FVector(0.f, 0.f, -96.f));

	isFPS = false;

	currentCam = GetFollowCamera();

	Health = 1.f;
}


//////////////////////////////////////////////////////////////////////////
// Input

void AProjectXCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("SwitchWeapon", IE_Pressed, this, &AProjectXCharacter::SwitchWeapon);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AProjectXCharacter::ReloadAction);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AProjectXCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AProjectXCharacter::StopFire);
	PlayerInputComponent->BindAction("IronSightView", IE_Pressed, this, &AProjectXCharacter::ActivateIronSightCam);
	PlayerInputComponent->BindAction("IronSightView", IE_Released, this, &AProjectXCharacter::DeactivateIronSightCam);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AProjectXCharacter::StartCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AProjectXCharacter::Run);
	PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &AProjectXCharacter::Walk);
	PlayerInputComponent->BindAction("Walk", IE_Released, this, &AProjectXCharacter::Run);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AProjectXCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AProjectXCharacter::Run);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AProjectXCharacter::PerformInteraction);
	PlayerInputComponent->BindAction("CameraSwap", IE_Pressed, this, &AProjectXCharacter::SwapCameras);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AProjectXCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AProjectXCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AProjectXCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AProjectXCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AProjectXCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AProjectXCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AProjectXCharacter::OnResetVR);
}

void AProjectXCharacter::ActivateIronSightCam()
{
	if (HoldingWeapon != nullptr)
	{

		if (HoldingWeapon != nullptr)
		{
			viewThroughScope = true;
			if (currentCam == FollowCamera)
				SwapCameras();
			GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(HoldingWeapon, 0.15f);
		}
	}
}

void AProjectXCharacter::DeactivateIronSightCam()
{
	viewThroughScope = false;
	GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(this, 0.15f);
}


void AProjectXCharacter::Walk()
{
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
}

void AProjectXCharacter::Sprint()
{
	GetCharacterMovement()->MaxWalkSpeed = sprintSpeed;
}

void AProjectXCharacter::Run()
{
	if (isCrouching)
		isCrouching = false;
	GetCharacterMovement()->MaxWalkSpeed = runSpeed;
}

void AProjectXCharacter::StartCrouch() {
	isCrouching = true;
	GetCharacterMovement()->MaxWalkSpeed = crouchSpeed;
}

void AProjectXCharacter::StartFire()
{
	if (HoldingWeapon != nullptr)
		if (HoldingWeapon->GetClass()->ImplementsInterface(UWeaponInterface::StaticClass()))
			HoldingWeapon->Execute_BeginFire(Cast<UObject>(HoldingWeapon), HoldingWeapon);
}

void AProjectXCharacter::StopFire()
{
	if (HoldingWeapon != nullptr)
		if (HoldingWeapon->GetClass()->ImplementsInterface(UWeaponInterface::StaticClass()))
			HoldingWeapon->Execute_EndFire(Cast<UObject>(HoldingWeapon), HoldingWeapon);
}

void AProjectXCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AProjectXCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AProjectXCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void AProjectXCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());

}

void AProjectXCharacter::LookUpAtRate(float Rate)
{

	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AProjectXCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);

	}
}

void AProjectXCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}


void AProjectXCharacter::BeginPlay()
{
	Super::BeginPlay();
	AnimInstance = Cast<UProjectXAnimInstance>(GetMesh()->GetAnimInstance());
	if (HealthHUD)
	{
		HealthHUD.GetDefaultObject()->Health = Health;
		Execute_ActivateHUD(Cast<UObject>(this), HealthHUD);
	}

}

void AProjectXCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DeltaSeconds = DeltaTime;

	EnableIK();
	IKUpdate();

	if (interactableList.Num() > 0)
	{
		GetBestInteractable();
		if (BestInteractable != nullptr)
		{
			AWeapon* weapon = Cast<AWeapon>(BestInteractable);
			if (weapon != nullptr && BestResult != -1 && weapon->Texture != nullptr)
			{
				ShowWeaponPickupHUD(WeaponPickupHUD, weapon->Texture, weapon->WeaponName);
			}
			else
				HideWeaponPickupHUD(WeaponPickupHUD);
		}
	}
	else
	{
		BestInteractable = nullptr;
		HideWeaponPickupHUD(WeaponPickupHUD);
	}

	if (Health < 1.f && Revive)
	{
		Health += 0.1;
		if (HealthHUD.GetDefaultObject())
		{
			HealthHUD.GetDefaultObject()->Health = Health;
			Execute_RefreshHUD(Cast<UObject>(this), HealthHUD);
		}
	}
	if (Health > 1.f)
		Health = 1.f;

}

void AProjectXCharacter::KillCharacter()
{
	Destroy();
}

float AProjectXCharacter::TakeDamage(float Damage, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::SanitizeFloat(Health));

	if (this->IsA<AProjectXPlayerCharacter>())
		Health -= Damage / 450.f;
	else
		Health -= Damage / 25.f;


	Revive = false;
	GetWorldTimerManager().ClearTimer(Handle);
	GetWorldTimerManager().SetTimer(Handle, this, &AProjectXCharacter::CanRevive, -3.f, false, 3.0f);

	if (HealthHUD.GetDefaultObject())
	{
		HealthHUD.GetDefaultObject()->Health = Health;
		Execute_RefreshHUD(Cast<UObject>(this), HealthHUD);
	}

	if (Health <= 0.f)
		Die();

	return ActualDamage;
}

void AProjectXCharacter::OpenGate()
{
	GATE = true;
}

void AProjectXCharacter::CloseGate()
{
	GATE = false;
}

bool AProjectXCharacter::IsMoving()
{

	return this->GetVelocity().Size() > 0.0f;
}

void AProjectXCharacter::EnableIK_Implementation()
{
	OpenGate();
}

void AProjectXCharacter::IKResetVars_Implementation()
{
	IKUpdateFootOffset(0.f, AnimInstance->RightEffectorLoc, IKFeetInterpSpeed);
	IKUpdateFootOffset(0.f, AnimInstance->LeftEffectorLoc, IKFeetInterpSpeed);
	IKUpdateFootRotation(FRotator(0.f, 0.f, 0.f), AnimInstance->RightFootRotation, IKFeetInterpSpeed);
	IKUpdateFootRotation(FRotator(0.f, 0.f, 0.f), AnimInstance->LeftFootRotation, IKFeetInterpSpeed);
	IKUpdateFootOffset(0.f, AnimInstance->HipsOffset, IKHipsInterpSpeed);
	UpdateCapsuleHalfHeight(0.f, false);
}

void AProjectXCharacter::DisableIK_Implementation()
{

	FTimerHandle UnusedHandle;
	GetWorldTimerManager().SetTimer(UnusedHandle, this, &AProjectXCharacter::CloseGate, IKTimeout, false);
}

void AProjectXCharacter::IKUpdate()
{
	if (GATE)
	{
		float LeftFootOffset;
		FVector Limpact;
		IKFootTrace(IKTraceDistance, IKLeftSocket, LeftFootOffset, Limpact);
		IKUpdateFootRotation(NormalToRotator(Limpact), AnimInstance->LeftFootRotation, IKFeetInterpSpeed);

		float RightFootOffset;
		FVector Rimpact;
		IKFootTrace(IKTraceDistance, IKRightSocket, RightFootOffset, Rimpact);
		IKUpdateFootRotation(NormalToRotator(Rimpact), AnimInstance->RightFootRotation, IKFeetInterpSpeed);

		float val = FMath::Min<float>(LeftFootOffset, RightFootOffset);
		float HipsOffset = val < 0.f ? val : 0.f;
		IKUpdateFootOffset(HipsOffset, AnimInstance->HipsOffset, IKHipsInterpSpeed);
		UpdateCapsuleHalfHeight(HipsOffset, false);

		IKUpdateFootOffset(LeftFootOffset - HipsOffset, AnimInstance->LeftEffectorLoc, IKFeetInterpSpeed);
		IKUpdateFootOffset(RightFootOffset - HipsOffset, AnimInstance->RightEffectorLoc, IKFeetInterpSpeed);

		bool left = FMath::Abs(AnimInstance->LeftEffectorLoc - LeftFootOffset + HipsOffset) < 1;
		bool right = FMath::Abs(AnimInstance->RightEffectorLoc - RightFootOffset + HipsOffset) < 1;
		if (left && right)
			DisableIK();

	}
}

void AProjectXCharacter::IKFootTrace(const float & TraceDistance, const FName & SocketName, float & Offset, FVector & impact)
{
	FName Socket = SocketName;
	float Distance = TraceDistance;


	FVector SockLoc(GetMesh()->GetSocketLocation(Socket));
	FVector ActorLoc(GetActorLocation());

	FVector src(SockLoc.X, SockLoc.Y, ActorLoc.Z);
	FVector dst(SockLoc.X, SockLoc.Y, ActorLoc.Z - IKCapsuleHalfHeight - Distance);

	FHitResult hitOut(ForceInit);

	GetWorld()->LineTraceSingleByChannel(hitOut, src, dst, ECC_Visibility);
	bool ValidHit = hitOut.IsValidBlockingHit();
	Offset = ValidHit ? (((hitOut.Location - hitOut.TraceEnd).Size() - Distance) + IKAdjustOffset) : 0.0f;
	impact = hitOut.Normal;

}

void AProjectXCharacter::IKUpdateFootRotation(const FRotator & TargetValue, FRotator & RotationVar, const float & InterpSpeed)
{
	FRotator Target = TargetValue;
	RotationVar = FMath::RInterpTo(RotationVar, Target, DeltaSeconds, InterpSpeed);
}

void AProjectXCharacter::IKUpdateFootOffset(const float & TargetValue, float & EffectorVar, const float & IntrpSpeed)
{
	float Target = TargetValue;
	EffectorVar = FMath::FInterpTo(EffectorVar, Target, DeltaSeconds, IntrpSpeed);
}

void AProjectXCharacter::UpdateCapsuleHalfHeight(const float & HipsShift, const bool & ResetDefault)
{
	float val = ResetDefault ? IKCapsuleHalfHeight : (IKCapsuleHalfHeight - (FMath::Abs(HipsShift) / 2));
	float interpVal = FMath::FInterpTo(GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), val, DeltaSeconds, IKHipsInterpSpeed);
	GetCapsuleComponent()->SetCapsuleHalfHeight(interpVal);

}

FRotator AProjectXCharacter::NormalToRotator(const FVector & Normal)
{
	float x = FMath::RadiansToDegrees(FMath::Atan2(Normal.Y, Normal.Z));
	float y = FMath::RadiansToDegrees(FMath::Atan2(Normal.X, Normal.Z));
	return FRotator(-y, 0.f, x);
}


void AProjectXCharacter::SwapCameras()
{
	auto x = GetFollowCamera();
	if (x->IsActive()) {
		x->Deactivate();
		isFPS = true;
		currentCam = FPSCam;
	}
	else {
		x->Activate();
		isFPS = false;
		currentCam = FollowCamera;
	}
}



void AProjectXCharacter::GetBestInteractable()
{
	BestResult = -1;
	BestInteractable = nullptr;

	for (AInteractable* i : interactableList)
	{
		FVector v = (i->meshComponent->GetCenterOfMass() - GetCurrentCamera()->GetComponentLocation());
		float result = FVector::DotProduct(v, GetActorForwardVector());
		if (result > 0.81f && result > BestResult)
		{
			BestResult = result;
			BestInteractable = i;
		}

	}
}

void AProjectXCharacter::Interact_Implementation(class AInteractable *interactable)
{
	interactableList.AddUnique(interactable);
}

void AProjectXCharacter::PickedUp_Implementation(class AWeapon *actor, class AProjectXCharacter* who)
{

}

void AProjectXCharacter::PutDown_Implementation(AWeapon * actor)
{
}

void AProjectXCharacter::PickedUpUpdatedPhysics_Implementation(AWeapon * actor)
{
	actor->AttachToComponent(this->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponHoldSocket);
}

void AProjectXCharacter::ReloadAction()
{
	if (HoldingWeapon != nullptr)
	{
		if (viewThroughScope)
			DeactivateIronSightCam();

		ReloadWeapon = true;
		if (HoldingWeapon != nullptr)
		{
			HoldingWeapon->canFire = false;
			if (HoldingWeapon->GetClass()->ImplementsInterface(UWeaponInterface::StaticClass()))
				HoldingWeapon->Execute_ReloadComplete(Cast<UObject>(HoldingWeapon));
		}
	}
}

void AProjectXCharacter::BeginFire_Implementation(AWeapon * weapon)
{
}

void AProjectXCharacter::EndFire_Implementation(AWeapon* weapon)
{

}

void AProjectXCharacter::Reload_Implementation(AWeapon * weapon)
{
	if (viewThroughScope)
		DeactivateIronSightCam();
	ReloadAction();
}

void AProjectXCharacter::ReloadComplete_Implementation()
{
	ReloadWeapon = false;
}

void AProjectXCharacter::FiringNotify_Implementation(AWeapon * weapon)
{
	if (weapon != nullptr)
		weapon->isFiring = true;
	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, this, &AProjectXCharacter::FireRecoilComplete, HoldingWeapon->RecoilPower, false, 0.2);
}

void AProjectXCharacter::FireRecoilComplete()
{
	if (HoldingWeapon)
	{


		AddControllerPitchInput(HoldingWeapon->RecoilPower / 4);


		//Add Camera Effect
		FPostProcessSettings  settings;

		settings.bOverride_SceneFringeIntensity = true;
		settings.SceneFringeIntensity = 0.0f;

		GetFPSCamera()->PostProcessSettings = settings;

		if (HoldingWeapon->ScopeHasFocusCapability)
		{
			settings.bOverride_DepthOfFieldMethod = true;
			settings.DepthOfFieldMethod = EDepthOfFieldMethod::DOFM_Gaussian;

			settings.bOverride_DepthOfFieldFocalRegion = true;
			settings.DepthOfFieldFocalRegion = 1.0f;

			settings.bOverride_DepthOfFieldFarBlurSize = true;
			settings.DepthOfFieldFarBlurSize = 32.f;

			settings.bOverride_DepthOfFieldNearBlurSize = true;
			settings.DepthOfFieldNearBlurSize = 0.f;

		}

		HoldingWeapon->ScopeCam->PostProcessSettings = settings;
	}

}

void AProjectXCharacter::NotifyActivateHUD_Implementation()
{
}

void AProjectXCharacter::NotifyDeactivateHUD_Implementation()
{
}

void AProjectXCharacter::ActivateHUD_Implementation(TSubclassOf<UMainHUD> HUD)
{
}

void AProjectXCharacter::DeactivateHUD_Implementation(TSubclassOf<UMainHUD> HUD)
{

}

void AProjectXCharacter::RefreshHUD_Implementation(TSubclassOf<class UMainHUD> HUD)
{
}

void AProjectXCharacter::SwitchWeapon()
{
	if ((HoldingWeapon != nullptr || Inventory != nullptr) && !SwitchNow)
	{
		if (HoldingWeapon != nullptr)
			HoldingWeapon->canFire = false;
		if (Inventory != nullptr)
			Inventory->canFire = false;
		if (viewThroughScope)
			DeactivateIronSightCam();
		SwitchNow = true;
	}

}

void AProjectXCharacter::DoSwitching()
{
	if (Inventory && !HoldingWeapon)
	{
		Inventory->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		Inventory->AttachToComponent(this->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponHoldSocket);

		HoldingWeapon = Inventory;
		Inventory = nullptr;
		handFree = false;

		if (HoldingWeapon != nullptr)
			if (HoldingWeapon->GetClass()->ImplementsInterface(UHUDInterface::StaticClass()))
				HoldingWeapon->Execute_NotifyActivateHUD(Cast<UObject>(HoldingWeapon));
	}
	else if (!Inventory  && HoldingWeapon)
	{
		HoldingWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		HoldingWeapon->AttachToComponent(this->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponRestSocket);

		if (HoldingWeapon != nullptr)
			if (HoldingWeapon->GetClass()->ImplementsInterface(UHUDInterface::StaticClass()))
				HoldingWeapon->Execute_NotifyDeactivateHUD(Cast<UObject>(HoldingWeapon));

		Inventory = HoldingWeapon;
		HoldingWeapon = nullptr;
		handFree = true;

	}
	else if (Inventory && HoldingWeapon)
	{
		Inventory->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		Inventory->AttachToComponent(this->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponHoldSocket);

		HoldingWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		HoldingWeapon->AttachToComponent(this->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponRestSocket);

		if (HoldingWeapon)
			if (HoldingWeapon->GetClass()->ImplementsInterface(UHUDInterface::StaticClass()))
				HoldingWeapon->Execute_NotifyDeactivateHUD(Cast<UObject>(HoldingWeapon));

		AWeapon* temp = HoldingWeapon;
		HoldingWeapon = Inventory;
		Inventory = temp;
		handFree = false;

		if (HoldingWeapon != nullptr)
			if (HoldingWeapon->GetClass()->ImplementsInterface(UHUDInterface::StaticClass()))
				HoldingWeapon->Execute_NotifyActivateHUD(Cast<UObject>(HoldingWeapon));
	}
	if (HoldingWeapon != nullptr)
		HoldingWeapon->canFire = true;
	if (Inventory != nullptr)
		Inventory->canFire = false;

}

float AProjectXCharacter::GetCurrentHealth()
{
	return Health;
}

void AProjectXCharacter::Die()
{

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->SetComponentTickEnabled(false);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);

	GetMesh()->SetCollisionProfileName(FName(TEXT("Ragdoll")));
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;



	this->DetachFromControllerPendingDestroy();

	if (HoldingWeapon)
	{
		HoldingWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		this->Execute_PutDown(Cast<UObject>(this), HoldingWeapon);
		if (HoldingWeapon->GetClass()->ImplementsInterface(UWeaponInterface::StaticClass()))
			HoldingWeapon->Execute_PutDown(Cast<UObject>(HoldingWeapon), HoldingWeapon);
		HoldingWeapon = nullptr;
	}

	if (Inventory)
	{
		Inventory->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		this->Execute_PutDown(Cast<UObject>(this), Inventory);
		if (Inventory->GetClass()->ImplementsInterface(UWeaponInterface::StaticClass()))
			Inventory->Execute_PutDown(Cast<UObject>(Inventory), Inventory);
		Inventory = nullptr;
	}

	GetWorldTimerManager().SetTimer(KillerHandle, this, &AProjectXCharacter::KillCharacter, 10, false, 10);
}

bool AProjectXCharacter::IsDead()
{
	return Health <= 0.0;
}

void AProjectXCharacter::CanRevive()
{
	Revive = true;
}

void AProjectXCharacter::UnInteract_Implementation(class AInteractable * actor)
{
	interactableList.Remove(actor);
}

void AProjectXCharacter::PerformInteraction()
{
	if (!SwitchNow)
	{
		if (handFree)
		{
			if (BestInteractable)
			{
				//Attach Interactable
				HoldingWeapon = Cast<AWeapon>(BestInteractable);
				if (HoldingWeapon->GetClass()->ImplementsInterface(UWeaponInterface::StaticClass()))
					HoldingWeapon->Execute_PickedUp(Cast<UObject>(HoldingWeapon), HoldingWeapon, this);
				handFree = false;

				// HUD 
				if (HoldingWeapon != nullptr)
					if (HoldingWeapon->GetClass()->ImplementsInterface(UHUDInterface::StaticClass()))
						HoldingWeapon->Execute_NotifyActivateHUD(Cast<UObject>(HoldingWeapon));

				if (HoldingWeapon != nullptr)
					HoldingWeapon->canFire = true;
				if (Inventory != nullptr)
					Inventory->canFire = true;
			}
		}
		else
		{
			if (HoldingWeapon && BestInteractable)
			{
				//HUD

				if (HoldingWeapon != nullptr)
					if (HoldingWeapon->GetClass()->ImplementsInterface(UHUDInterface::StaticClass()))
						HoldingWeapon->Execute_NotifyDeactivateHUD(Cast<UObject>(HoldingWeapon));

				//Attach Interactable
				HoldingWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				this->Execute_PutDown(Cast<UObject>(this), HoldingWeapon);
				if (HoldingWeapon->GetClass()->ImplementsInterface(UWeaponInterface::StaticClass()))
					HoldingWeapon->Execute_PutDown(Cast<UObject>(HoldingWeapon), HoldingWeapon);

				//Attach Interactable
				HoldingWeapon = Cast<AWeapon>(BestInteractable);
				if (HoldingWeapon->GetClass()->ImplementsInterface(UWeaponInterface::StaticClass()))
					HoldingWeapon->Execute_PickedUp(Cast<UObject>(HoldingWeapon), HoldingWeapon, this);
				handFree = false;

				// HUD 
				if (HoldingWeapon != nullptr)
					if (HoldingWeapon->GetClass()->ImplementsInterface(UHUDInterface::StaticClass()))
						HoldingWeapon->Execute_NotifyActivateHUD(Cast<UObject>(HoldingWeapon));

				if (HoldingWeapon != nullptr)
					HoldingWeapon->canFire = true;
				if (Inventory != nullptr)
					Inventory->canFire = true;
			}
		}
	}

}
