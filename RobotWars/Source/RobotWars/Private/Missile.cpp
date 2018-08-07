// Fill out your copyright notice in the Description page of Project Settings.

#include "Missile.h"
#include "PaperSpriteComponent.h"
#include "RobotWarsStatics.h"
#include "Components/ArrowComponent.h"


// Sets default values
AMissile::AMissile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RobotBase"));
	}

	MissileDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("RobotDirection"));
	MissileDirection->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

	MissileSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("RobotSprite"));
	MissileSprite->AttachToComponent(MissileDirection, FAttachmentTransformRules::KeepWorldTransform);


}

void AMissile::SetInitialCondition(FVector InitialPosition, float Direction)
{
	MissileDirection->SetWorldRotation(FRotator(0.0f, Direction, 0.0f));
}

// Called when the game starts or when spawned
void AMissile::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Set the direction the missile will be facing.
	float CurrentHeading = FMath::DegreesToRadians(MissileDirection->GetComponentRotation().Yaw);
	FVector DesiredMovementDirection = FVector(MISSILE_SPEED * FMath::Cos(CurrentHeading), MISSILE_SPEED * FMath::Sin(CurrentHeading), 0.0f);
	
	//Set the rotation of the missile.
	MissileDirection->SetWorldRotation(DesiredMovementDirection.Rotation());

	//Get a unit vector facing where the missile is facing.
	FVector MovementDirection = MissileDirection->GetForwardVector();
	//Get the current location of the missile.
	FVector Pos = GetActorLocation();
	//Add the travel distance of the missile during DeltaTime to it's current location.
	//We don't move the Z position since it's a 2D game and Z is the hight.
	Pos.X += MovementDirection.X * MISSILE_SPEED * DeltaTime;
	Pos.Y += MovementDirection.Y * MISSILE_SPEED * DeltaTime;
	//Set the missile new position.
	SetActorLocation(Pos);
}

void AMissile::OnHit()
{
}

