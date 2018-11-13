// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Robot.h"
#include "Maximilian.generated.h"

/**
 * 
 */
UCLASS()
class ROBOTWARS_API AMaximilian : public ARobot
{
	GENERATED_BODY()
	
public:
	AMaximilian();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	int target = 3;
	
	
};
