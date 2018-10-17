// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#define MAX_SENSORS			4
#define NUM_ENERGY_SYSTEMS	4
#define BUMP_WALL			0x01
#define BUMP_ROBOT			0x02
#define BUMP_MISSILE		0x04
#define BUMP_LASER			0x08

typedef enum
{
	SYSTEM_SHIELDS, SYSTEM_SENSORS,
	SYSTEM_LASERS, SYSTEM_MISSILES
} SYSTEM;

typedef enum {
	SENSOR_RADAR, SENSOR_RANGE,
	SENSOR_NONE
} SENSORTYPE;

typedef enum {
	WEAPON_MISSILE,
	WEAPON_LASER,
	WEAPON_NONE
} WEAPONTYPE;

typedef enum {
	DAMAGE_MISSILE,
	DAMAGE_LASER,
	DAMAGE_ROBOT,
	DAMAGE_WALL
}DAMAGETYPE;

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
