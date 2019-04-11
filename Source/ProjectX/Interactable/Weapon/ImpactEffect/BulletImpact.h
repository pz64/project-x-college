// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletImpact.generated.h"

UCLASS()
class PROJECTX_API ABulletImpact : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = BulletImpact)
		class UParticleSystemComponent* HitEffect;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = BulletImpact)
		class USphereComponent* SphereCollision;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = BulletImpact)
		class URadialForceComponent* RadialForce;


public:
	// Sets default values for this actor's properties
	ABulletImpact();

	void PerformImpact(const FHitResult& HitResult, float Damage, AController* Shooter);

	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Life)
		float Lifetime;

	void SelfDestroy();
};
