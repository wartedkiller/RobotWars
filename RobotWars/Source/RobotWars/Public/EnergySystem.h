// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RobotWarsEnum.h"
#include "EnergySystem.generated.h"

//TODO Change Energy cost value to /sec instead of /min since DeltaTime is in sec?
#define MAX_GENERATOR_STRUCTURE		500
#define GENERATOR_CAPACITY			1400
#define MAX_SHIELD_ENERGY			1000
#define MAX_SHIELD_CHARGE_RATE		600
#define MAX_LASER_CHARGE_RATE		500
#define MAX_LASER_ENERGY			50
#define MIN_LASER_ENERGY			20
#define MAX_MISSILE_CHARGE_RATE		600
#define MAX_MISSILE_ENERGY			100
#define RANGE_SENSOR_ENERGY_COST	50
#define RADAR_SENSOR_ENERGY_COST	75

/**
 * 
 */
UCLASS()
class ROBOTWARS_API UEnergySystem : public UObject
{
	GENERATED_BODY()
public:
	UEnergySystem();

	int32 GetGeneratorStructur();
	int32 GetGeneratorOutput();
	int32 SetSystemChargePriorites(SYSTEM priorities[NUM_ENERGY_SYSTEMS]);
	float GetSystemEnergy(SYSTEM type);
	void SetSystemChargeRate(SYSTEM type, int32 rate);
	void UpdateEnergySystem(float DeltaTime, SENSORTYPE SensorArray[MAX_SENSORS]);


	///********** VARIABLE **************************************************************************************************************************************************
private:
	int32 CurrentEnergy[NUM_ENERGY_SYSTEMS];
	int32 CurrentGeneratorStructure;
	int32 EnergyChargeRatePerSecond[NUM_ENERGY_SYSTEMS];

	TArray<SYSTEM> SystemPriority;
};

