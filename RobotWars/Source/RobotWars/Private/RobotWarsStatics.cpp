// Fill out your copyright notice in the Description page of Project Settings.

#include "RobotWarsStatics.h"

float URobotWarsStatics::FindDeltaAngleDegrees(float A1, float A2)
{
	float Delta = A2 - A1;

	if (Delta > 180.0f)
	{
		Delta = Delta - (360.0f);
	}
	else if (Delta < -180.0f)
	{
		Delta = Delta + (360.0f);
	}

	return Delta;
}

float URobotWarsStatics::PositiveAngle(float Angle)
{
	while (Angle < 0)
	{
		Angle += 360.0f;
	}

	return Angle;
}
