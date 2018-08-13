// Fill out your copyright notice in the Description page of Project Settings.

#include "Missile.h"
#include "PaperSpriteComponent.h"
#include "RobotWarsStatics.h"
#include "Components/ArrowComponent.h"
#include "Engine/World.h"
#include "Robot.h"


// Sets default values
AMissile::AMissile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Radius = 6.5f;

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

	//Get current location of the missile
	FVector CurrentLocation = GetActorLocation();
	//Add the distance flown by the missile during DeltaTime
	FVector DesiredEndLocation = CurrentLocation + ((DeltaTime * MISSILE_SPEED) * GetTransform().GetUnitAxis(EAxis::X));
	
	//Check for collision
	if (UWorld* World = GetWorld())
	{
		FHitResult OutHit;
		FCollisionShape CollisionShape;
		CollisionShape.SetCapsule(Radius, 200.0f);

		//If the missile collide, handle the collision based on what it hit
		//Else move the missile forward
		if (World->SweepSingleByProfile(OutHit, CurrentLocation, DesiredEndLocation, FQuat::Identity, MovementCollisionProfile, CollisionShape))
		{
			if (OutHit.GetActor()->GetName().Compare(this->GetOwner()->GetName()) == 0)
			{
				SetActorLocation(DesiredEndLocation);
				UE_LOG(LogTemp, Warning, TEXT("Missile collided with its owner"))
			}
			else
			{
				SetActorLocation(OutHit.Location);
				UE_LOG(LogTemp, Warning, TEXT("Missile collided with : %s"), *OutHit.GetActor()->GetName())

				ARobot* HitRobot = Cast<ARobot>(OutHit.GetActor());
				HitRobot->GetHit();
				Explode();
			}
			
		}
		else
		{
			//Set Missile new location
			SetActorLocation(DesiredEndLocation);
			
		}
	}
}

void AMissile::Explode()
{
	UE_LOG(LogTemp, Warning, TEXT("Insert Missile Explosion Here"))
	
	Destroy();
}

