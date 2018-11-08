// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Robot.h"
#include "Bender.generated.h"

/**
 * 
 */
UCLASS()
class ROBOTWARS_API ABender : public ARobot
{
	GENERATED_BODY()
	
public:
	ABender();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;


	
	
};
