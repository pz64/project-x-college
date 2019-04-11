// Fill out your copyright notice in the Description page of Project Settings.

#include "HideSpotData.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "Character/ProjectXCharacter.h"
#include "Interactable/HideSpots/AttackOrHideBehavior.h"
#include "Runtime/Engine/Classes/Components/BillboardComponent.h"
#include "Runtime/Engine/Classes/Engine/Texture2D.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Public/TimerManager.h"

// Sets default values
AHideSpotData::AHideSpotData()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	BoxCollision->SetRelativeScale3D(FVector(2.f, 2.f, 2.f));

	SpriteComp = CreateDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));
	SpriteComp->SetupAttachment(BoxCollision);

	RootComponent = BoxCollision;

	Availability = true;
	AllocatedCharacter = nullptr;

	counter = 0;
	
}

// Called when the game starts or when spawned
void AHideSpotData::BeginPlay()
{
	Super::BeginPlay();

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AHideSpotData::OnOverlapBegin);
	BoxCollision->OnComponentEndOverlap.AddDynamic(this, &AHideSpotData::OnOverlapEnd);

	InitialLocation = this->GetActorLocation();
	this->SetActorTickEnabled(false);
}

void AHideSpotData::RemoveTick()
{
	this->SetActorLocation(InitialLocation);
	this->SetActorTickEnabled(false);
}

void AHideSpotData::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	HiddenActor = Cast<AProjectXCharacter>(OtherActor);
	Availability = false;
	this->SetActorTickEnabled(true);
}

void AHideSpotData::OnOverlapEnd(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	HiddenActor = nullptr;
	Availability = true;


	FTimerHandle h;
	GetWorldTimerManager().SetTimer(h, this, &AHideSpotData::RemoveTick, 1.f, false, 5.0f);
}

// Called every frame
void AHideSpotData::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector CurrentLocation(this->GetActorLocation());
	FVector NextLocation;

	NextLocation.Z = CurrentLocation.Z;
	NextLocation.X = CurrentLocation.X;
	NextLocation.Y = CurrentLocation.Y + FMath::Sin(counter) * 200;

	FVector TargetLocation = UKismetMathLibrary::VInterpTo(CurrentLocation, NextLocation, DeltaTime, 5.f);

	this->SetActorLocation(TargetLocation);

	counter = counter > 10000.0f ? 0 : counter + 0.1;

	if (FVector::Dist(InitialLocation, NextLocation) > 400.f)
		CurrentLocation = InitialLocation;

}

void AHideSpotData::SetHideSpotName(const FName & Spot)
{
	HSpotName = Spot;
}

FName AHideSpotData::GetSpotName()
{
	return HSpotName;
}

AProjectXCharacter * AHideSpotData::GetHiddenActor()
{
	return HiddenActor;
}
