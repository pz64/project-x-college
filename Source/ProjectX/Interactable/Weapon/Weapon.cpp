// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "HUD/MainHUD.h"
#include "HUD/WeaponCounterHUD.h"
#include "HUD/CrosshairHUD.h"
#include "Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Interactable/Weapon/ImpactEffect/BulletImpact.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/Texture2D.h"
#include "EngineUtils.h"
#include "Character/AI/SupportAI/AIBehavior.h"
#include "Character/AI/EnemyAI/EnemyAIBehavior.h"
#include "Character/ProjectXCharacter.h"
#include "Components/CapsuleComponent.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	MuzzleSocket = "Muzzle";

	ScopeCam = CreateDefaultSubobject<UCameraComponent>(TEXT("ScopeCam"));
	MuzzleFlash = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Muzzle flash"));
	MuzzleFlash->SetupAttachment(meshComponent);
	ScopeCam->SetupAttachment(meshComponent);
	//init
	RecoilTime = 0.5;
	RecoilPower = 1;
	MagazineSize = MagazineSizeMax = 60;
	MagazineCount = 4;
	canFire = true;
	HitResult.Init();
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	FindAllActors<AAIBehavior>(GetWorld(), groupbehavior);
	if (groupbehavior.Num() > 0)
		for (AAIBehavior* GroupBehaviour : groupbehavior)
			GroupBehaviour->RegisterWeapon(this);
	FindAllActors<AEnemyAIBehavior>(GetWorld(), enemyGroupbehavior);
	if (enemyGroupbehavior.Num() > 0)
		for (AEnemyAIBehavior* GroupBehaviour : enemyGroupbehavior)
			GroupBehaviour->RegisterWeapon(this);
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::FireOnce()
{
	if (Shooter != nullptr && canFire)
	{
		MagazineCount = MagazineCount <= 0 ? 0 : MagazineCount;
		if (MagazineSize <= 0)
		{
			MagazineSize = 0;
			if (MagazineCount > 0)
				if (Shooter->GetClass()->ImplementsInterface(UWeaponInterface::StaticClass()))
					Shooter->Execute_Reload(Cast<UObject>(Shooter), this);
		}
		if (MagazineSize > 0 && !Shooter->ReloadWeapon && !Shooter->IsDead())
		{
			MagazineSize--;
			BulletCounterHUD.GetDefaultObject()->MagSizeMax = MagazineSizeMax;
			BulletCounterHUD.GetDefaultObject()->MagSize = MagazineSize;
			BulletCounterHUD.GetDefaultObject()->MagCount = MagazineCount;

			if (Shooter->GetClass()->ImplementsInterface(UHUDInterface::StaticClass()))
			{
				Shooter->Execute_RefreshHUD(Cast<UObject>(Shooter), BulletCounterHUD);
				Shooter->Execute_RefreshHUD(Cast<UObject>(Shooter), CrosshairHUD);
			}

			//linetrace

			FVector src = Shooter->viewThroughScope ? ScopeCam->GetComponentLocation() : Shooter->GetCurrentCamera()->GetComponentLocation();
			FVector dst = FRotationMatrix(Shooter->viewThroughScope ? ScopeCam->GetComponentRotation() : Shooter->GetCurrentCamera()->GetComponentRotation()).GetScaledAxis(EAxis::X) * 999999999.f + src;

			FCollisionQueryParams traceqparam;
			traceqparam.AddIgnoredActor(this);
			traceqparam.AddIgnoredActor(Shooter);

			TArray<AProjectXCharacter*> Characters;
			FindAllActors<AProjectXCharacter>(GetWorld(), Characters);

			for (int32 i = 0; i < Characters.Num(); ++i)
				traceqparam.AddIgnoredComponent(Characters[i]->GetCapsuleComponent());

			bool successfulHit = GetWorld()->LineTraceSingleByChannel(HitResult, src, dst, ECollisionChannel::ECC_Camera, traceqparam);

			if (Shooter->GetClass()->ImplementsInterface(UWeaponInterface::StaticClass()))
				Shooter->Execute_FiringNotify(Cast<UObject>(Shooter), this);

			if (FlashParticle != nullptr)
			{
				UGameplayStatics::SpawnEmitterAttached(
					FlashParticle,
					meshComponent,
					MuzzleSocket,
					FVector(0, 0, 0),
					FRotator(0, 0, 0),
					EAttachLocation::KeepRelativeOffset,
					true //auto delete on completion
				);
			}

			if (successfulHit && ImpactForce != nullptr)
			{
				FTimerHandle Handle;


				FVector MuzzleLoc = meshComponent->GetSocketLocation(FName("Muzzle"));
				float dist = FVector::Dist(MuzzleLoc, HitResult.ImpactPoint);
				float time = dist / BulletVelocity;

				GetWorldTimerManager().SetTimer(Handle, this, &AWeapon::SpawnImapact, 1.0f, false, time);



			}

			//Add Recoil Effect
			Shooter->AddControllerPitchInput(-RecoilPower);

			//Add Camera Effect
			FPostProcessSettings  settings;

			settings.bOverride_SceneFringeIntensity = true;
			settings.SceneFringeIntensity = RecoilPower*3.5;

			Shooter->GetFPSCamera()->PostProcessSettings = settings;

			if (ScopeHasFocusCapability)
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


			ScopeCam->PostProcessSettings = settings;

		}

	}
}

void AWeapon::SetShooter(AProjectXCharacter * Shooter)
{
	this->Shooter = Shooter;
}

void AWeapon::SpawnImapact()
{
	FActorSpawnParameters SpawnInfo;
	ABulletImpact* BulletImpact = GetWorld()->SpawnActor<ABulletImpact>(ImpactForce, HitResult.ImpactPoint, UKismetMathLibrary::FindLookAtRotation(HitResult.ImpactPoint, HitResult.ImpactNormal), SpawnInfo);
	if (BulletImpact && Shooter)
	{
		BulletImpact->PerformImpact(HitResult, Damage, Shooter->GetController());
	}

}

void AWeapon::FiringNotify_Implementation(AWeapon * weapon)
{
}

void AWeapon::BeginFire_Implementation(AWeapon * weapon)
{
	GetWorldTimerManager().SetTimer(Handle, this, &AWeapon::FireOnce, RecoilTime, true, 0);
}

void AWeapon::EndFire_Implementation(AWeapon* weapon)
{
	GetWorldTimerManager().ClearTimer(Handle);
}

void AWeapon::Reload_Implementation(AWeapon * weapon)
{
}

void AWeapon::ReloadComplete_Implementation()
{

	if (MagazineCount > 0)
	{
		canFire = true;
		MagazineSize = MagazineSizeMax;
		MagazineCount--;

		BulletCounterHUD.GetDefaultObject()->MagSizeMax = MagazineSizeMax;
		BulletCounterHUD.GetDefaultObject()->MagSize = MagazineSize;
		BulletCounterHUD.GetDefaultObject()->MagCount = MagazineCount;

		if (Shooter->GetClass()->ImplementsInterface(UHUDInterface::StaticClass()))
			Shooter->Execute_RefreshHUD(Cast<UObject>(Shooter), BulletCounterHUD);
	}
}

void AWeapon::NotifyActivateHUD_Implementation()
{
	if (Shooter != nullptr)
	{
		if (Shooter->GetClass()->ImplementsInterface(UHUDInterface::StaticClass()))
		{
			Shooter->Execute_ActivateHUD(Cast<UObject>(Shooter), BulletCounterHUD);
			Shooter->Execute_ActivateHUD(Cast<UObject>(Shooter), CrosshairHUD);

			BulletCounterHUD.GetDefaultObject()->MagSizeMax = MagazineSizeMax;
			BulletCounterHUD.GetDefaultObject()->MagSize = MagazineSize;
			BulletCounterHUD.GetDefaultObject()->MagCount = MagazineCount;

			Shooter->Execute_RefreshHUD(Cast<UObject>(Shooter), BulletCounterHUD);
			Shooter->Execute_RefreshHUD(Cast<UObject>(Shooter), CrosshairHUD);
		}
	}
}

void AWeapon::NotifyDeactivateHUD_Implementation()
{
	if (Shooter != nullptr)
	{
		if (Shooter->GetClass()->ImplementsInterface(UHUDInterface::StaticClass()))
		{
			Shooter->Execute_DeactivateHUD(Cast<UObject>(Shooter), BulletCounterHUD);
			Shooter->Execute_DeactivateHUD(Cast<UObject>(Shooter), CrosshairHUD);
		}

	}
}

void AWeapon::ActivateHUD_Implementation(TSubclassOf<UMainHUD> HUD)
{
}

void AWeapon::DeactivateHUD_Implementation(TSubclassOf<UMainHUD> HUD)
{
}

void AWeapon::RefreshHUD_Implementation(TSubclassOf<class UMainHUD> HUD)
{
}

void AWeapon::PickedUp_Implementation(class AWeapon * actor, class AProjectXCharacter* who)
{
	SetShooter(who);
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, "Pickedup");
	canInteract = false;
	actor->meshComponent->SetSimulatePhysics(false);
	actor->SetActorEnableCollision(false);

	if (who->GetClass()->ImplementsInterface(UWeaponInterface::StaticClass()))
		who->Execute_PickedUpUpdatedPhysics(Cast<UObject>(who), actor);
}

void AWeapon::PutDown_Implementation(AWeapon * actor)
{
	SetShooter(nullptr);
	canFire = false;
	actor->SetActorEnableCollision(true);
	actor->meshComponent->SetSimulatePhysics(true);
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, "PutDown");
	canInteract = true;
}

void AWeapon::PickedUpUpdatedPhysics_Implementation(AWeapon * actor)
{
}

template<typename T>
void FindAllActors(UWorld* World, TArray<T*>& Out)
{
	for (TActorIterator<AActor> It(World, T::StaticClass()); It; ++It)
	{
		T* Actor = Cast<T>(*It);
		if (Actor && !Actor->IsPendingKill())
		{
			Out.Add(Actor);
		}
	}
}
