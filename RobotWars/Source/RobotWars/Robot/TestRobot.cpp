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

	if (GetSystemEnergy(SYSTEM_MISSILES) == 100) {
		FireWeapon(WEAPON_MISSILE, 0);
	}
	if (GetSystemEnergy(SYSTEM_LASERS) >= 45)
	{
		FireWeapon(WEAPON_LASER, 0);
	}
}

void ATestRobot::BeginPlay()
{
	Super::BeginPlay();

	SetMotorSpeeds(50, 50);

	SetSystemChargeRate(SYSTEM_SHIELDS, 600);
	SetSystemChargeRate(SYSTEM_LASERS, 200);
	SetSystemChargeRate(SYSTEM_MISSILES, 300);
	
	AddSensor(1, SENSOR_RADAR, 0, 45, 100);
	AddSensor(0, SENSOR_RANGE, 0, 0, 125);
}
