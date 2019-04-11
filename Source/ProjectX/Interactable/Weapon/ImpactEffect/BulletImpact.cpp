// Fill out your copyright notice in the Description page of Project Settings.
#include "BulletImpact.h"
#include "Runtime/Engine/Classes/Components/SphereComponent.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/GameFramework/DamageType.h"
#include "Runtime/Engine/Classes/PhysicsEngine/RadialForceComponent.h"
// Sets default values
ABulletImpact::ABulletImpact()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	HitEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle Effect"));
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RadialForce = CreateDefaultSubobject<URadialForceComponent>(TEXT("Impact Force"));

	RadialForce->SetupAttachment(SphereCollision);
	HitEffect->SetupAttachment(SphereCollision);
	RootComponent = SphereCollision;

	RadialForce->bAutoActivate = true;
	RadialForce->ImpulseStrength = 10000;
	RadialForce->ForceStrength = 10000;
	RadialForce->Radius = 10;

	HitEffect->bAutoActivate = true;
	HitEffect->bAutoDestroy = true;

	Lifetime = 3.0f;
}

void ABulletImpact::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
	//	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, SweepResult.BoneName.ToString());
	}


}

void ABulletImpact::PerformImpact(const FHitResult& Hit, float Damage, AController* Shooter)
{
	if (Hit.Actor.Get())
	{
		FHitResult HitResult;
		UGameplayStatics::ApplyPointDamage(Hit.GetActor(), Damage, Hit.ImpactPoint, HitResult, Shooter, this, UDamageType::StaticClass());

	}
}


// Called when the game starts or when spawned
void ABulletImpact::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, this, &ABulletImpact::SelfDestroy, Lifetime, false);

	SphereCollision->OnComponentBeginOverlap.AddDynamic(this , &ABulletImpact::OnOverlapBegin);

}

// Called every frame
void ABulletImpact::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABulletImpact::SelfDestroy()
{
	if (!Destroy())
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "Can't destroy this BS!!");
}

