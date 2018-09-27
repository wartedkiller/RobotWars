// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RobotWarsEnum.h"
#include "Components/SceneComponent.h"
#include "SensorSystem.generated.h"

#define RANGE_MAX_RANGE  125
#define RADAR_MIN_RANGE   32
#define RADAR_MAX_RANGE  100
#define MIN_RADAR_ARC     10
#define MAX_RADAR_ARC     45

/**
 * 
 */
UCLASS()
class ROBOTWARS_API USensorSystem : public USceneComponent
{
	GENERATED_BODY()

		///********** METHOD **************************************************************************************************************************************************
public:
	USensorSystem();
	int32 AddSensor(SENSORTYPE type, int32 angle, int32 width, int32 range);
	SENSORTYPE GetTypeOfSensor();
	void SetIsEnoughEnergy(bool status);
	void SetSensorData(int32 data);
	void SetSensorStatus(bool status);
	bool IsEnoughEnergy();
	bool IsSensorOn();
	int32 GetSensorAngle();
	int32 GetSensorWidth();
	int32 GetSensorRange();
	int32 GetSensorData();

		///********** VARIABLE **************************************************************************************************************************************************
private:
	bool bSensorOn;
	bool bEnoughEnergy;
	SENSORTYPE SensorType;
	int32 SensorAngle;
	int32 SensorWidth;
	int32 SensorRange;
	int32 SensorData;
};
