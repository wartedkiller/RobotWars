// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Robot.h"
#include "TestRobot.generated.h"

/**
 * 
 */
UCLASS()
class ROBOTWARS_API ATestRobot : public ARobot
{
	GENERATED_BODY()
	
public:
	ATestRobot();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
protected:
	virtual void BeginPlay() override;
	
	int32 target = 3;
};
