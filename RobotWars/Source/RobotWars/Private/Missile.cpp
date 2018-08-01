// Fill out your copyright notice in the Description page of Project Settings.

#include "Missile.h"
#include "PaperSpriteComponent.h"
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
	SetActorLocation(InitialPosition);
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

}

