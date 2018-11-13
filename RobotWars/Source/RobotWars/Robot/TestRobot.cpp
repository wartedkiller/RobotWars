// Fill out your copyright notice in the Description page of Project Settings.

#include "TestRobot.h"

ATestRobot::ATestRobot()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ATestRobot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GPS_INFO gpsData = GetGPSInfo();
	float targetX = 0, targetY = 0, dx = 0, dy = 0, angle = 0, dist = 0;
	char statusMessage[STATUS_MSG_LEN];

	UE_LOG(LogTemp, Warning, TEXT("TestRobot: X = %f    Y = %f    Heading = %f"), gpsData.x, gpsData.y, gpsData.heading);

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
	//if (angle < 0)
	//{
	//	angle += 360;
	//}

	//angle = angle - gpsData.heading;

	//if (dx > 0)
	//{
	//	if (dy < 0)
	//	{
	//		angle = 270 + angle;
	//	}
	//}

	UE_LOG(LogTemp, Warning, TEXT("Maximilian: dx = %f    dy = %f"), dx, dy);

	if (FMath::Abs(gpsData.heading - angle) < 2.5)
	{
		UE_LOG(LogTemp, Warning, TEXT("Go Straight"));
	}
	else if (angle > -2.5)
	{
		UE_LOG(LogTemp, Warning, TEXT("Go Right"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Go Left"));
	}

	sprintf_s(statusMessage,
		"Target: %d\nDesired Angle: %5.2f\nActual Angle: %5.2f\nDistance: %5.2f\n",
		target, angle, gpsData.heading, dist);
	SetStatusMessage(statusMessage);
}

void ATestRobot::BeginPlay()
{
	Super::BeginPlay();

	SetMotorSpeeds(0, 0);

	//SetSystemChargeRate(SYSTEM_SHIELDS, 600);
	//SetSystemChargeRate(SYSTEM_LASERS, 500);
	//SetSystemChargeRate(SYSTEM_MISSILES, 600);
	
	AddSensor(1, SENSOR_RADAR, 0, 45, 100);
	AddSensor(0, SENSOR_RANGE, 40, 0, 125);
	AddSensor(2, SENSOR_RANGE, 140, 0, 125);
}
