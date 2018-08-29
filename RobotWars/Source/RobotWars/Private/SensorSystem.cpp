// Fill out your copyright notice in the Description page of Project Settings.

#include "SensorSystem.h"

USensorSystem::USensorSystem()
{
	MyType = SENSOR_NONE;
	SensorAngle = -1;
	SensorWidth = -1;
	SensorRange = -1;
	bIsSensorOn = false;
	bIsEnoughEnergy = false;
}

int32 USensorSystem::AddSensor(SENSORTYPE type, int32 angle, int32 width, int32 range)
{
	MyType = type;
	
	SensorAngle = angle % 360;
	
	if (type == SENSOR_RADAR)
	{
		if (width < MIN_RADAR_ARC)
		{
			width = MIN_RADAR_ARC;
		}
		else if (width > MAX_RADAR_ARC)
		{
			width = MAX_RADAR_ARC;
		}
	}
	SensorWidth = width;
	if (type == SENSOR_RANGE)
	{
		if (range > RANGE_MAX_RANGE)
		{
			range = RANGE_MAX_RANGE;
		}
	}
	else if (type == SENSOR_RADAR)
	{
		if (range < RADAR_MIN_RANGE)
		{
			range = RADAR_MIN_RANGE;
		} 
		else if (range > RADAR_MAX_RANGE)
		{
			range = RADAR_MAX_RANGE;
		}
	}
	SensorRange = range;
	bIsSensorOn = true;
	bIsEnoughEnergy = true;

	return 1;
}

SENSORTYPE USensorSystem::GetTypeOfSensor()
{
	return MyType;
}

void USensorSystem::SetIsEnoughEnergy(bool status)
{
	bIsEnoughEnergy = status;
}

bool USensorSystem::IsEnoughEnergy()
{
	return bIsEnoughEnergy;
}

bool USensorSystem::IsSensorOn()
{
	return bIsSensorOn;
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
