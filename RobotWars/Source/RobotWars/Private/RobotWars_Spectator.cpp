// Fill out your copyright notice in the Description page of Project Settings.

#include "RobotWars_Spectator.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/ArrowComponent.h"
#include "Camera/CameraComponent.h"


// Sets default values
ARobotWars_Spectator::ARobotWars_Spectator()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Creating a USceneComponent as the RootComponent of the Robot since
	//USceneComponent are the lightest object that keep track of it's position.
	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SpectatorBase"));
	}

	//This will be remove and placed in the spectator.
	///If something is changed to the SpringArm or the CameraComponent, you must restart UE4
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetArmLength = 500.0f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = false;
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->CameraLagSpeed = 2.0f;
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));

	//This will be remove and placed in the spectator.
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SpectatorCamera"));
	CameraComponent->bUsePawnControlRotation = false;
	CameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;

	//Set the resolution and aspect ratio of the camera. It is set to 1280.0 width with an aspect ratio of 16:9 so the height is 720.0 (720p).
	CameraComponent->OrthoWidth = 1280.0f;
	CameraComponent->AspectRatio = 16.0f / 9.0f;
	CameraComponent->SetupAttachment(SpringArm, SpringArm->SocketName);
	CameraComponent->SetWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));
}

// Called when the game starts or when spawned
void ARobotWars_Spectator::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARobotWars_Spectator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ARobotWars_Spectator::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

