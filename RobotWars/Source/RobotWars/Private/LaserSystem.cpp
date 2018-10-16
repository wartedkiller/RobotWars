// Fill out your copyright notice in the Description page of Project Settings.

#include "LaserSystem.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"

ULaserSystem::ULaserSystem()
{
	static ConstructorHelpers::FObjectFinder<UClass> LaserBP(TEXT("Class'/Game/Blueprint/Laser_BP.Laser_BP_C'"));
	if (LaserBP.Succeeded())
	{
		LaserToSpawn = LaserBP.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Missile Blueprint could not be found so NO MISSILE WILL SPAWN"))
	}
}

bool ULaserSystem::Fire(AActor * Robot, FVector Location, FRotator Heading, float Damage)
{
	if (LaserToSpawn)
	{
		UWorld* World = Robot->GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = Robot;

			ALaser* NewLaser = World->SpawnActor<ALaser>(LaserToSpawn, Location, Heading, SpawnParams);
			NewLaser->SetDamage(Damage);
			return true;
		}
	}
	return false;
}
