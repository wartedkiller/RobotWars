// Fill out your copyright notice in the Description page of Project Settings.

#include "Bender.h"

ABender::ABender()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ABender::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);

	//Add your robot actions here.  Remember, since this isn't your own main()
	//that you'll need to make any variables that need to have a memory
	//static so they'll preserve their value from call to call.
	//I TOLD YOU there was a use for it!

	int32 frontRange, backRange;
	int32 numViews = 0, laserHits = 0;
	char statusMessage[STATUS_MSG_LEN];
	SYSTEM prios[NUM_ENERGY_SYSTEMS] = { SYSTEM_MISSILES, SYSTEM_SENSORS,
		SYSTEM_SHIELDS, SYSTEM_LASERS };
	SetSystemChargePriorites(prios);

	SetSystemChargeRate(SYSTEM_SHIELDS, 200);
	SetSystemChargeRate(SYSTEM_MISSILES, 600);

	UE_LOG(LogTemp, Warning, TEXT("Sensor 0 data = %d"), GetSensorData(0));
	UE_LOG(LogTemp, Warning, TEXT("Sensor 1 data = %d"), GetSensorData(1));
	UE_LOG(LogTemp, Warning, TEXT("Sensor 2 data = %d"), GetSensorData(2));
	UE_LOG(LogTemp, Warning, TEXT("Sensor 3 data = %d"), GetSensorData(3));

	if (GetSensorData(1) > 0)
		SetMotorSpeeds(25, 25);
	else if (GetSensorData(0) > 0) {
		if (!IsTurboOn())
			TurboBoost();
		SetMotorSpeeds(75, 75);
	}
	else {
		frontRange = GetSensorData(2);
		backRange = GetSensorData(3);
		if (frontRange < 80 && backRange < 80)
			SetMotorSpeeds(60, 100);
		else if (frontRange > backRange)
			SetMotorSpeeds(100, 70);
		else if (frontRange < backRange)
			SetMotorSpeeds(70, 100);
		else
			SetMotorSpeeds(100, 100);
	}
	if (GetSystemEnergy(SYSTEM_MISSILES) == MIN_MISSILE_ENERGY)
		FireWeapon(WEAPON_MISSILE, 0);

	if (GetBumpInfo() & BUMP_LASER)
		laserHits++;

	sprintf_s(statusMessage,
		"You just proved robot\nadvertising works! %d views!\n"
		"\nTimes hit by laser: %d", numViews++, laserHits);
	SetStatusMessage(statusMessage);
}

void ABender::BeginPlay()
{
	Super::BeginPlay();

	SetRobotName("Bender");

	AddSensor(0, SENSOR_RADAR, -10, 20, 100);
	AddSensor(1, SENSOR_RADAR, -20, 40, 60);
	AddSensor(2, SENSOR_RANGE, 40, 0, 0);
	AddSensor(3, SENSOR_RANGE, 140, 0, 0);

}
