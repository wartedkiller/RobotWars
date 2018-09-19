// Fill out your copyright notice in the Description page of Project Settings.

#include "SensorSystem.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

USensorSystem::USensorSystem()
{
	SensorType = SENSOR_NONE;
	SensorAngle = -1;
	SensorWidth = -1;
	SensorRange = -1;
	bSensorOn = false;
	bEnoughEnergy = false;
	SensorData = 0;
}

int32 USensorSystem::AddSensor(SENSORTYPE type, int32 angle, int32 width, int32 range)
{
	SensorType = type;
	
	SensorAngle = angle % 360;

	SensorWidth = width;
	SensorRange = range;
	bSensorOn = true;
	bEnoughEnergy = true;

	return 1;
}

SENSORTYPE USensorSystem::GetTypeOfSensor()
{
	return SensorType;
}

void USensorSystem::SetIsEnoughEnergy(bool status)
{
	bEnoughEnergy = status;
}

bool USensorSystem::IsEnoughEnergy()
{
	return bEnoughEnergy;
}

bool USensorSystem::IsSensorOn()
{
	return bSensorOn;
}

int32 USensorSystem::GetSensorAngle()
{
	return SensorAngle;
}

int32 USensorSystem::GetSensorWidth()
{
	return SensorWidth;
}

int32 USensorSystem::GetSensorRange()
{
	return SensorRange;
}
