// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RobotWarsStatics.generated.h"

/**
 * 
 */
UCLASS()
class ROBOTWARS_API URobotWarsStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Math")
	static float FindDeltaAngleDegrees(float A1, float A2);

	UFUNCTION(BlueprintCallable, Category = "Math")
	static float PositiveAngle(float Angle);
};
