// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Robot.h"
#include "DummyRobot.generated.h"

/**
 * 
 */
UCLASS()
class ROBOTWARS_API ADummyRobot : public ARobot
{
	GENERATED_BODY()
	
public:
	ADummyRobot();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	
	
};
