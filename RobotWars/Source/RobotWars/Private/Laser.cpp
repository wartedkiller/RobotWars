// Fill out your copyright notice in the Description page of Project Settings.

#include "Laser.h"
#include "PaperSpriteComponent.h"
#include "RobotWarsStatics.h"
#include "Components/ArrowComponent.h"
#include "Engine/World.h"
#include "Robot.h"

// Sets default values
ALaser::ALaser()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Radius = 6.5f;

	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("LaserBase"));
	}

	LaserDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("LaserDirection"));
	LaserDirection->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

	LaserSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("LaserSprite"));
	LaserSprite->AttachToComponent(LaserDirection, FAttachmentTransformRules::KeepWorldTransform);

	MovementCollisionProfile = TEXT("Projectile:Fly");

}

// Called when the game starts or when spawned
void ALaser::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALaser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Get current location of the missile
	FVector CurrentLocation = GetActorLocation();
	//Add the distance flown by the missile during DeltaTime
	FVector DesiredEndLocation = CurrentLocation + ((DeltaTime * MISSILE_SPEED) * GetTransform().GetUnitAxis(EAxis::X));


	//Check for collision
	if (UWorld* World = GetWorld())
	{
		TArray<FHitResult> OutHit;
		FCollisionShape CollisionShape;
		CollisionShape.SetCapsule(Radius, 200.0f);

		//If the missile collide, handle the collision based on what it hit
		//Else move the missile forward
		if (World->SweepMultiByProfile(OutHit, CurrentLocation, DesiredEndLocation, FQuat::Identity, MovementCollisionProfile, CollisionShape))
		{
			for (int32 CurrentCollision = 0; CurrentCollision < OutHit.Num(); CurrentCollision++)
			{
				if (OutHit[CurrentCollision].GetActor()->GetName().Compare(this->GetOwner()->GetName()) == 0 || OutHit[CurrentCollision].GetActor()->GetName().Compare(this->GetName()) == 0)
				{
					SetActorLocation(DesiredEndLocation);
				}
				else
				{
					SetActorLocation(OutHit[CurrentCollision].Location);

					//TODO Check for splash damage from the explosion.
					if (ARobot* HitRobot = Cast<ARobot>(OutHit[CurrentCollision].GetActor()))
					{
						HitRobot->GetHit(LASER, MISSILE_DAMAGE);
						Explode();
						break;
					}
				}
			}
		}
		else
		{
			//Set Missile new location
			SetActorLocation(DesiredEndLocation);
		}
	}
}

void ALaser::SetDamage(int32 Damage)
{
	LaserDamage = Damage;
}

void ALaser::Explode()
{
	UE_LOG(LogTemp, Warning, TEXT("Laser hit for %i damage"), LaserDamage)

		Destroy();
}

