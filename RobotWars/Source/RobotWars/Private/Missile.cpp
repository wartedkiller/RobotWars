// Fill out your copyright notice in the Description page of Project Settings.

#include "Missile.h"
#include "PaperSpriteComponent.h"
#include "RobotWarsStatics.h"
#include "Components/ArrowComponent.h"
#include "Engine/World.h"
#include "Robot.h"
#include "Arena.h"
#include "Components/CapsuleComponent.h"
#include "UObject/ConstructorHelpers.h"


/****************************************************
IF ANYTHING CHANGE IN THE CONSTRUCTOR OF THIS CLASS,
THE ENGINE MUST BE RESTARTED TO SEE THE CHANGE.
FAILING TO DO SO WILL RESULT IN SEEING WEIRD RESULT
OF WHAT YOU ARE TRYING TO DO.
****************************************************/
AMissile::AMissile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Radius = 6.5f;

	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("MissileBase"));
	}

	MissileDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("MissileDirection"));
	MissileDirection->SetupAttachment(RootComponent);

	MissileSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("MissileSprite"));
	MissileSprite->SetupAttachment(MissileDirection);

	MovementCollisionProfile = TEXT("Projectile:Fly");

	static ConstructorHelpers::FObjectFinder<UClass> ExplosionBP(TEXT("Blueprint'/Game/Blueprint/Explosion/MissileExplosion_BP.MissileExplosion_BP_C'"));
	if (ExplosionBP.Succeeded())
	{
		ExplosionActor = ExplosionBP.Object;
	}

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
		TArray<FHitResult> OutHit;
		FCollisionShape CollisionShape;
		CollisionShape.SetCapsule(Radius, 200.0f);

		//If the missile collide, handle the collision based on what it hit
		//Else move the missile forward
		if (World->SweepMultiByProfile(OutHit, CurrentLocation, DesiredEndLocation, FQuat::Identity, MovementCollisionProfile, CollisionShape))
		{
			for (FHitResult CurrentCollision : OutHit)
			{
				if (Cast<AArena>(CurrentCollision.GetActor()))
				{
					ActorHitByMissile = CurrentCollision.GetActor();
					Explode(CurrentCollision.Location);
					break;
				}
				if (CurrentCollision.GetActor()->GetName().Compare(this->GetOwner()->GetName()) == 0 || CurrentCollision.GetActor()->GetName().Compare(this->GetName()) == 0)
				{
					SetActorLocation(DesiredEndLocation);
				}
				else
				{
					//SetActorLocation(OutHit[CurrentCollision].Location);

					if (ARobot* HitRobot = Cast<ARobot>(CurrentCollision.GetActor()))
					{
						ActorHitByMissile = HitRobot;
						HitRobot->GetHit(DAMAGE_MISSILE, MISSILE_DAMAGE);
						((ARobot*)this->GetOwner())->Score += MISSILE_DAMAGE;
						Explode(CurrentCollision.Location);
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

//TODO Add damage based on distance of the explosion.
void AMissile::Explode(FVector ExplosionLocation)
{

	this->PlayExplosionSound();
	//Check for Splash damage.
	//Check for collision
	if (UWorld* World = GetWorld())
	{
		TArray<FHitResult> OutHit;
		FCollisionShape CollisionShape;
		CollisionShape.SetCapsule(MISSILE_EXPLOSION_RADIUS, MISSILE_EXPLOSION_RADIUS);

		if (World->SweepMultiByProfile(OutHit, GetActorLocation(), GetActorLocation(), FQuat::Identity, MovementCollisionProfile, CollisionShape))
		{
			for (FHitResult CurrentCollision : OutHit)
			{
				if (ARobot* HitRobot = Cast<ARobot>(CurrentCollision.GetActor()))
				{
					if (Cast<UCapsuleComponent>(CurrentCollision.GetComponent()))
					{
						FVector DistanceVector = CurrentCollision.Location - ExplosionLocation;
						//Setting the FVector Z axis to 40.0f set it at the same height it was before de substraction.
						DistanceVector.Z = 40.0f;
						float Distance = DistanceVector.Size();
						float SplashDamage = (Distance / MISSILE_EXPLOSION_RADIUS) * MISSILE_SPLASH_DAMAGE;
						HitRobot->GetHit(DAMAGE_MISSILE, SplashDamage);

						if (ActorHitByMissile != nullptr)
						{
							if (CurrentCollision.GetActor()->GetName().Compare(this->GetOwner()->GetName()) != 0)
							{
								Cast<ARobot>(GetOwner())->Score += SplashDamage;
							}
						}
					}
				}
			}
		}

		//Spawning Missile Explosion
		FActorSpawnParameters SpawnParams;
		World->SpawnActor<AExplosionActor>(ExplosionActor, GetActorLocation(), MissileDirection->GetComponentRotation(), SpawnParams);

	}
	Destroy();
}

