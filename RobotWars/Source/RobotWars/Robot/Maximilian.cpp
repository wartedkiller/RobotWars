// Fill out your copyright notice in the Description page of Project Settings.

#include "Maximilian.h"

AMaximilian::AMaximilian()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AMaximilian::Tick(float DeltaTime)
{
	//THIS NEED TO BE THE FIRST LINE IN YOUR TICK() FUNCTION
	Super::Tick(DeltaTime);

	//Add your robot actions here.  Remember, since this isn't your own main()
	//that you'll need to make any variables that need to have a memory
	//static so they'll preserve their value from call to call.
	//I TOLD YOU there was a use for it!

	UE_LOG(LogTemp, Warning, TEXT("Sensor 0 Data = %i"), GetSensorData(0));
	UE_LOG(LogTemp, Warning, TEXT("Sensor 1 Data = %i"), GetSensorData(1));

	GPS_INFO gpsData;
	int target = 0;
	int targetX = 0;
	int targetY = 0;
	float dx, dy, dist, angle;
	char statusMessage[STATUS_MSG_LEN];

	SetSystemChargeRate(SYSTEM_SHIELDS, 400);
	SetSystemChargeRate(SYSTEM_LASERS, 500);

	if (GetSensorData(2) && GetSensorData(1)) {
		SetMotorSpeeds(0, 0);
		SetStatusMessage("Firing!");
		if (GetSystemEnergy(SYSTEM_LASERS) >= MIN_LASER_ENERGY)
			FireWeapon(WEAPON_LASER, 0);
	}
	else if (GetSensorData(2)) {
		SetMotorSpeeds(-20, 20);
		SetStatusMessage("Aiming!");
	}
	else if (GetSensorData(1)) {
		SetMotorSpeeds(20, -20);
		SetStatusMessage("Aiming!");
	}
	else {
		gpsData = GetGPSInfo();
		switch (target) {
		case 0:
			targetX = -457, targetY = 310;
			break;
		case 1:
			targetX = 457, targetY = -310;
			break;
		case 2:
			targetX = -457, targetY = -310;
			break;
		case 3:
			targetX = 457, targetY = 310;
			break;

		}
		dx = targetX - gpsData.x;
		dy = targetY - gpsData.y;

		dist = FMath::Sqrt(FMath::Square(dx) + FMath::Square(dy));
		if (dist < 10) {
			target += 1 % 4;
		}
		angle = FMath::Asin(dy / dist);
		angle = FMath::RadiansToDegrees(angle);

		UE_LOG(LogTemp, Warning, TEXT("Maximilian: dx = %f    dy = %f"), dx, dy);

		/*if (dy < 0) {
			if (dx < 0)
				angle = -angle;
			else
				angle = 180 + angle;
		}
		else if (dx > 0)
			angle = angle + 180;
		else
			angle = 360 - angle;*/

		if (FMath::Abs(gpsData.heading - angle) < 2.5)
			SetMotorSpeeds(100, 100);
		else if (gpsData.heading < angle)
			SetMotorSpeeds(100, 60);
		else
			SetMotorSpeeds(60, 100);

		sprintf_s(statusMessage,
			"Target: %d\nDesired Angle: %5.2f\nActual Angle: %5.2f\nDistance: %5.2f\n",
			target, angle, gpsData.heading, dist);
		SetStatusMessage(statusMessage);
	}

}

void AMaximilian::BeginPlay()
{
	//THIS NEED TO BE THE FIRST LINE IN YOUR BEGINPLAY() FUNCTION
	Super::BeginPlay();

	AddSensor(2, SENSOR_RADAR, -15, 12, 100);
	AddSensor(1, SENSOR_RADAR, 3, 12, 100);
}
