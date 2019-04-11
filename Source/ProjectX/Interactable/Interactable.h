// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableInterface.h"
#include "Interactable.generated.h"

UCLASS()
class PROJECTX_API AInteractable : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AInteractable();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Interactable)
		class USphereComponent* collisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Interactable)
		class USkeletalMeshComponent* meshComponent;

	UPROPERTY(EditAnywhere, Category = Interactable)
		float sphereScale;

	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/////////////////
	//				//
	//				//
	// Interactable	//
	//				//
	//				//
	/////////////////

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Interactable)
		void Interact(class AInteractable *actor);
	virtual void Interact_Implementation(class AInteractable *actor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Interactable)
		void UnInteract(class AInteractable *actor);
	virtual void UnInteract_Implementation(class AInteractable *actor);
	bool canInteract;



	/////////////////
	//				//
	//				//
	// Common Fns	//
	//				//
	//				//
	/////////////////

	UPROPERTY(EditAnywhere, Category = Interactable)
		FString ActorName;
};

