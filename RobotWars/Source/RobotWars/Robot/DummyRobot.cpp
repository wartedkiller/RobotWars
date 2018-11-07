// Fill out your copyright notice in the Description page of Project Settings.

#include "DummyRobot.h"

ADummyRobot::ADummyRobot()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ADummyRobot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADummyRobot::BeginPlay()
{
	Super::BeginPlay();

	AddSensor(1, SENSOR_RADAR, 0, 45, 100);
	AddSensor(0, SENSOR_RANGE, 0, 0, 125);
}


