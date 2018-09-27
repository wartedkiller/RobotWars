// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#define MAX_SENSORS			4
#define NUM_ENERGY_SYSTEMS	4

typedef enum
{
	SYSTEM_SHIELDS, SYSTEM_SENSORS,
	SYSTEM_LASERS, SYSTEM_MISSILES
} SYSTEM;

typedef enum {
	SENSOR_RADAR, SENSOR_RANGE,
	SENSOR_NONE
} SENSORTYPE;

typedef struct {
	float x;
	float y;
	float heading;
} GPS_INFO;

/**
 * 
 */
class ROBOTWARS_API RobotWarsEnum
{
public:
	RobotWarsEnum();
	~RobotWarsEnum();
};
