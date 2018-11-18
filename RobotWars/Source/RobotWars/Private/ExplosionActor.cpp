// Fill out your copyright notice in the Description page of Project Settings.

#include "ExplosionActor.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "UObject/ConstructorHelpers.h"


// Sets default values
AExplosionActor::AExplosionActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ExplosionBase"));
	}

	ExplosionFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("RobotExplosionFlipbook"));
	ExplosionFlipbook->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AExplosionActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExplosionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

