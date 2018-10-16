// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RobotWarsEnum.h"
#include "Laser.h"
#include "LaserSystem.generated.h"

/**
 * 
 */
UCLASS()
class ROBOTWARS_API ULaserSystem : public UObject
{
	GENERATED_BODY()
	
///********** METHOD ***********************************************************************************************************************************************

public:
	ULaserSystem();
	bool Fire(class AActor* Robot, FVector Location, FRotator Heading, float Damage);
	
///********** VARIABLE ***********************************************************************************************************************************************

public:
	SYSTEM SystemType = SYSTEM_LASERS;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Laser", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<ALaser>LaserToSpawn;

};
