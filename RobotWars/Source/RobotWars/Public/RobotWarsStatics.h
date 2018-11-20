// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RobotWarsStatics.generated.h"

#define STATUS_MSG_LEN	150
#define M_PI 3.14159265358979323846

//Robots constants
#define MAX_TREAD_SPEED			100.0f
#define MIN_TREAD_SPEED			-100.0f
#define MAX_SPEED				100.0f				//In UnrealUnit/s
#define TREAD_DISTANCE			15.0f
#define TURBO_ENERGY_COST		100
#define GPS_ENERGY_COST			2
#define TURBO_TIME				3.0f
#define TURBO_MAX_SPEED			125.0f
#define TURBO_SPEED				75.0f
#define ARENA_FRICTION			40.0f
#define BASE_RECOIL_VELOCITY	80.0f

//EnergySystem constants
#define MAX_GENERATOR_STRUCTURE		500
#define GENERATOR_CAPACITY			1400
#define SHIELDS_LEAK_THRESHOLD		600
#define MAX_SHIELD_ENERGY			1000
#define MAX_SHIELD_CHARGE_RATE		600
#define MAX_LASER_CHARGE_RATE		500
#define MAX_LASER_ENERGY			50
#define MIN_LASER_ENERGY			20
#define MAX_MISSILE_CHARGE_RATE		600
#define MAX_MISSILE_ENERGY			100
#define MIN_MISSILE_ENERGY			100
#define RANGE_SENSOR_ENERGY_COST	50.0f
#define RADAR_SENSOR_ENERGY_COST	75.0f

/**
 * 
 */
UCLASS()
class ROBOTWARS_API URobotWarsStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Math")
	static float FindDeltaAngleDegrees(float A1, float A2);

	UFUNCTION(BlueprintCallable, Category = "Math")
	static float PositiveAngle(float Angle);
};
