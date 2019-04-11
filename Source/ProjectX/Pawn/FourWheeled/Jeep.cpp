// Fill out your copyright notice in the Description page of Project Settings.

#include "Jeep.h"
#include "RearWheel.h"
#include "FrontWheel.h"
#include "GameFramework/Controller.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "Engine/SkeletalMesh.h"
#include "JeepAnimInstance.h"


AJeep::AJeep()
{

	UWheeledVehicleMovementComponent4W* MovementComp = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());
	check(MovementComp->WheelSetups.Num() == 4);

	MovementComp->WheelSetups[0].WheelClass = UFrontWheel::StaticClass();
	MovementComp->WheelSetups[0].BoneName = FName("front_left");
	MovementComp->WheelSetups[0].AdditionalOffset = FVector(0.f, 0.f, 0.f);

	MovementComp->WheelSetups[1].WheelClass = UFrontWheel::StaticClass();
	MovementComp->WheelSetups[1].BoneName = FName("front_right");
	MovementComp->WheelSetups[1].AdditionalOffset = FVector(0.f, 0.f, 0.f);

	MovementComp->WheelSetups[2].WheelClass = URearWheel::StaticClass();
	MovementComp->WheelSetups[2].BoneName = FName("rear_left");
	MovementComp->WheelSetups[2].AdditionalOffset = FVector(0.f, 0.f, 0.f);

	MovementComp->WheelSetups[3].WheelClass = URearWheel::StaticClass();
	MovementComp->WheelSetups[3].BoneName = FName("rear_right");
	MovementComp->WheelSetups[3].AdditionalOffset = FVector(0.f, 0.f, 0.f);


	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetOffset = FVector(0.f, 0.f, 200.f);
	SpringArm->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 600.0f;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraRotationLagSpeed = 7.f;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritRoll = false;

	// Create camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
	Camera->FieldOfView = 90.f;
}

void AJeep::SetupPlayerInputComponent(UInputComponent * InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
	check(InputComponent);

	InputComponent->BindAxis("MoveForward", this, &AJeep::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AJeep::MoveRight);
	InputComponent->BindAxis("LookUp");
	InputComponent->BindAxis("LookRight");

	InputComponent->BindAction("Jump", IE_Pressed, this, &AJeep::OnHandbrakePressed);
	InputComponent->BindAction("Jump", IE_Released, this, &AJeep::OnHandbrakeReleased);

}

void AJeep::BeginPlay()
{
	Super::BeginPlay();
}


void AJeep::Tick(float Delta)
{
	Super::Tick(Delta);
}



void AJeep::MoveForward(float Val)
{
	GetVehicleMovementComponent()->SetThrottleInput(Val);
}

void AJeep::UpdatePhysicsMaterial()
{
	
}

void AJeep::MoveRight(float Val)
{
	GetVehicleMovement()->SetSteeringInput(Val);
}

void AJeep::OnHandbrakePressed()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
}

void AJeep::OnHandbrakeReleased()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
}
