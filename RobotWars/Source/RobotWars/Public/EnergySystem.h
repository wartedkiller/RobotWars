// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RobotWarsEnum.h"
#include "SensorSystem.h"
#include "Robot.h"
#include "EnergySystem.generated.h"

//TODO Change Energy cost value to /sec instead of /min since DeltaTime is in sec? It cost a float division everytime I use a /min cost.
//TODO Create a method iin32/bool RemoveEnergyFromShield(int32 EnergyToRemove); to remove Energy Point just from the shield for the GPS and the TurboBoost.

/**
 * 
 */
UCLASS()
class ROBOTWARS_API UEnergySystem : public UObject
{
	GENERATED_BODY()

		///********** METHOD **************************************************************************************************************************************************

public:
	UEnergySystem();

	int32 GetGeneratorStructure();
	int32 GetGeneratorOutput();
	int32 SetSystemChargePriorites(SYSTEM priorities[NUM_ENERGY_SYSTEMS]);
	float GetWeaponDamage(WEAPONTYPE Type);
	float GetSystemEnergy(SYSTEM type);
	void SetSystemChargeRate(SYSTEM type, int32 rate);
	void UpdateEnergySystem(float DeltaTime, ARobot* robot);
	bool RemoveEnergy(int32 Value);
	bool DamageShield(int32 value);


	///********** VARIABLE **************************************************************************************************************************************************
private:
	float CurrentEnergy[NUM_ENERGY_SYSTEMS];
	float CurrentGeneratorStructure;
	int32 EnergyChargeRatePerSecond[NUM_ENERGY_SYSTEMS];

	TArray<SYSTEM> SystemPriority;
};

