// Fill out your copyright notice in the Description page of Project Settings.

#include "MissileSystem.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"

/****************************************************
IF ANYTHING CHANGE IN THE CONSTRUCTOR OF THIS CLASS,
THE ENGINE MUST BE RESTARTED TO SEE THE CHANGE.
FAILING TO DO SO WILL RESULT IN SEEING WEIRD RESULT
OF WHAT YOU ARE TRYING TO DO.
****************************************************/
UMissileSystem::UMissileSystem()
{
	static ConstructorHelpers::FObjectFinder<UClass> MissileBP(TEXT("Class'/Game/Blueprint/Missile_BP.Missile_BP_C'"));
	if (MissileBP.Succeeded())
	{
		MissileToSpawn = MissileBP.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Missile Blueprint could not be found so NO MISSILE WILL SPAWN"))
	}
}

bool UMissileSystem::Fire(class AActor* Robot, FVector Location, FRotator Heading)
{
	if (MissileToSpawn)
	{
		UWorld* World = Robot->GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = Robot;

			AMissile* temp = World->SpawnActor<AMissile>(MissileToSpawn, Location, Heading, SpawnParams);
			if (temp)
			{
				UE_LOG(LogTemp, Warning, TEXT("Missile spawned correctly"))
			}
			UE_LOG(LogTemp, Warning, TEXT("Missile Fire"))
			return true;
		}
	}
	return false;
}


