// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HideSpotData.generated.h"

UENUM(BlueprintType)
enum class EHideSpotType : uint8
{
	ATTACK_SPOT,
	HIDE_SPOT
};

UCLASS()
class PROJECTX_API AHideSpotData : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = HideSpotData)
		class UBoxComponent* BoxCollision;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = HideSpotData)
		FName HSpotName;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = HideSpotData)
		class AProjectXCharacter* HiddenActor;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = HideSpotData)
		class UBillboardComponent* SpriteComp;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = HideSpotData)
		bool Availability;
	class AProjectXAICharacter* AllocatedCharacter;

public:
	// Sets default values for this actor's properties
	AHideSpotData();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = HideSpotData)
		EHideSpotType HideSpotType;

	float counter; // movement
	FVector InitialLocation;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void RemoveTick();

	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void SetHideSpotName(const FName& Spot);

	UFUNCTION(BlueprintCallable)
		FName GetSpotName();

	UFUNCTION(BlueprintCallable)
		class AProjectXCharacter* GetHiddenActor();
};

template<typename T>
void FindAllActors(UWorld* World, TArray<T*>& Out);

