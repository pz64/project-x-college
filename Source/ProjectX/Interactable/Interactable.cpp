// Fill out your copyright notice in the Description page of Project Settings.

#include "Interactable.h"
#include "Character/ProjectXCharacter.h"
#include "Runtime/Engine/Classes/Components/SphereComponent.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"

// Sets default values
AInteractable::AInteractable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	sphereScale = 120.f;

	meshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("mesh"));
	collisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Spherecollision"));
	collisionSphere->InitSphereRadius(sphereScale);
	collisionSphere->SetupAttachment(meshComponent);

	RootComponent = meshComponent;

	collisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AInteractable::OnOverlapBegin);
	collisionSphere->OnComponentEndOverlap.AddDynamic(this, &AInteractable::OnOverlapEnd);

	// INTERACTION
	canInteract = true;
	meshComponent->SetCollisionProfileName(FName("Ragdoll"));
	SetActorEnableCollision(true);
	meshComponent->SetSimulatePhysics(true);


}

void AInteractable::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && canInteract)
		if (OtherActor->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
			Cast<IInteractableInterface>(OtherActor)->Execute_Interact(Cast<UObject>(OtherActor), this);

}

void AInteractable::OnOverlapEnd(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
		if (OtherActor->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
			Cast<IInteractableInterface>(OtherActor)->Execute_UnInteract(Cast<UObject>(OtherActor), this);
}


void AInteractable::Interact_Implementation(class AInteractable * actor)
{
}

void AInteractable::UnInteract_Implementation(class AInteractable * actor)
{

}

// Called when the game starts or when spawned
void AInteractable::BeginPlay()
{
	Super::BeginPlay();

}



// Called every frame
void AInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

