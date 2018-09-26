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
}

void ATestRobot::BeginPlay()
{
	Super::BeginPlay();

	SetMotorSpeeds(0, 0);
	FireMissile();
	SetSystemChargeRate(SYSTEM_SHIELDS, 600);
	SetSystemChargeRate(SYSTEM_LASERS, 200);
	SetSystemChargeRate(SYSTEM_MISSILES, 300);
	
	AddSensor(1, SENSOR_RADAR, 0, 45, 100);
}
