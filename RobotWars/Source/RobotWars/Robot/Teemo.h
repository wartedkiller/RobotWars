// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Robot.h"
#include "Teemo.generated.h"

/**
 * 
 */
UCLASS()
class ROBOTWARS_API ATeemo : public ARobot
{
	GENERATED_BODY()

public:
	ATeemo();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	void Shield_Sensor_Charge();
	void Mode_Chasse();
	void Mode_Combat();
	void Mode_Escape();
	void Mode_Search();
	void Case_Select(int teemo_case);
	void Teemo_Attack();
	int Teemo_Targetting();
	int Teemo_Case();


	int teemo_case = 0;

};
