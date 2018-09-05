// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Missile.h"
#include "RobotWarsEnum.h"
#include "MissileSystem.generated.h"

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		//UPROPERTY(EditAnywhere)
		TSubclassOf<AMissile> MissileToSpawn;
	
};