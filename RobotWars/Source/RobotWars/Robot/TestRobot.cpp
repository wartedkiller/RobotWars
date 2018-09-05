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

	SetTreadSpeed(30, -30);
	FireMissile();
	SetSystemChargeRate(SYSTEM_SHIELDS, 600);
	
	AddSensor(0, SENSOR_RANGE, 90, 0, 0);
	AddSensor(1, SENSOR_RANGE, -90, 0, 0);
	AddSensor(2, SENSOR_RANGE, 180, 0, 0);
	AddSensor(3, SENSOR_RANGE, 0, 0, 0);
}