// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Missile.h"
#include "RobotWarsEnum.h"
#include "MissileSystem.generated.h"

#define MISSILE_MAX_ANGLE	90

/**
 * 
 */
UCLASS()
class ROBOTWARS_API UMissileSystem : public UObject
{
	GENERATED_BODY()

///********** METHOD ***********************************************************************************************************************************************

public:
	UMissileSystem();
	bool Fire(class AActor* Robot, FVector Location, FRotator Heading);


///********** VARIABLE ***********************************************************************************************************************************************

public:
	SYSTEM SystemType = SYSTEM_MISSILES;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Missile", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<AMissile> MissileToSpawn;
	
};
