// Fill out your copyright notice in the Description page of Project Settings.

#include "EnergySystem.h"

UEnergySystem::UEnergySystem()
{
	CurrentGeneratorStructure = MAX_GENERATOR_STRUCTURE;

	for (int32 i = 0; i < NUM_ENERGY_SYSTEMS - 1; i++)
	{
		CurrentEnergy[i] = 0;
		EnergyChargeRatePerSecond[i] = 0;
	}

	SystemPriority.Add(SYSTEM_SHIELDS);
	SystemPriority.Add(SYSTEM_SENSORS);
	SystemPriority.Add(SYSTEM_LASERS);
	SystemPriority.Add(SYSTEM_MISSILES);
}

int32 UEnergySystem::GetGeneratorStructur()
{
	return CurrentGeneratorStructure;
}

int32 UEnergySystem::GetGeneratorOutput()
{
	return (CurrentGeneratorStructure / MAX_GENERATOR_STRUCTURE) * GENERATOR_CAPACITY;
}

int32 UEnergySystem::SetSystemChargePriorites(SYSTEM priorities[NUM_ENERGY_SYSTEMS])
{
	TArray<SYSTEM> temp;

	for (int32 i = 0; i < NUM_ENERGY_SYSTEMS - 1; i++)
	{
		if (temp.AddUnique(priorities[i]) == -1)
		{
			return 0;
		}
	}

	SystemPriority = temp;
	return 1;
}

float UEnergySystem::GetSystemEnergy(SYSTEM type)
{
	switch (type)
	{
	case SYSTEM_SHIELDS:
		return CurrentEnergy[SYSTEM_SHIELDS];
		break;
	case SYSTEM_LASERS:
		return CurrentEnergy[SYSTEM_LASERS];
		break;
	case SYSTEM_MISSILES:
		return CurrentEnergy[SYSTEM_MISSILES];
		break;
	default:
		return -1;
		break;
	}
}

void UEnergySystem::SetSystemChargeRate(SYSTEM type, int32 rate)
{
	switch (type)
	{
	case SYSTEM_SHIELDS:
		if (rate > MAX_SHIELD_CHARGE_RATE)
		{
			rate = MAX_SHIELD_CHARGE_RATE;
		}
		EnergyChargeRatePerSecond[type] = rate / 60;
		break;
	case SYSTEM_LASERS:
		if (rate > MAX_LASER_CHARGE_RATE)
		{
			rate = MAX_LASER_CHARGE_RATE;
		}
		EnergyChargeRatePerSecond[type] = rate / 60;
		break;
	case SYSTEM_MISSILES:
		if (rate > MAX_MISSILE_CHARGE_RATE)
		{
			rate = MAX_MISSILE_CHARGE_RATE;
		}
		EnergyChargeRatePerSecond[type] = rate / 60;
		break;
	default:
		break;
	}
}


//TODO Update Shield and Sensor based on Energy. Probably be done in the robot.
void UEnergySystem::UpdateEnergySystem(float DeltaTime, SENSORTYPE SensorArray[MAX_SENSORS])
{
	TArray<SYSTEM> temp = SystemPriority;

	float EnergyToSpendThisTurn = (CurrentGeneratorStructure / MAX_GENERATOR_STRUCTURE) * (GENERATOR_CAPACITY / 60) * DeltaTime;

	for (SYSTEM CurrentSystem : SystemPriority)
	{
		//Stop giving energy if there is no energy to give.
		if (EnergyToSpendThisTurn == 0)
		{
			break;
		}

		//Find which System to give Energy.
		switch (CurrentSystem)
		{
		case SYSTEM_SHIELDS:
			//If the System is fully charged, don't try to give it Energy.
			if (!(CurrentEnergy[SYSTEM_SHIELDS] >= MAX_SHIELD_ENERGY))
			{
				if (EnergyToSpendThisTurn >= EnergyChargeRatePerSecond[SYSTEM_SHIELDS])
				{
					EnergyToSpendThisTurn -= EnergyChargeRatePerSecond[SYSTEM_SHIELDS];
					CurrentEnergy[SYSTEM_SHIELDS] += EnergyChargeRatePerSecond[SYSTEM_SHIELDS];
				}
				else
				{
					CurrentEnergy[SYSTEM_SHIELDS] += EnergyToSpendThisTurn;
					EnergyToSpendThisTurn = 0;
				}

				//Check for extra energy in system. If system is overcharged
				//return the extra energy to the pool of available energy this turn.
				if (CurrentEnergy[SYSTEM_SHIELDS] > MAX_SHIELD_ENERGY)
				{
					float ExtraEnergy = CurrentEnergy[SYSTEM_SHIELDS] - MAX_SHIELD_ENERGY;
					CurrentEnergy[SYSTEM_SHIELDS] = MAX_SHIELD_ENERGY;
					EnergyToSpendThisTurn += ExtraEnergy;
				}
			}
			break;
		case SYSTEM_SENSORS:
			for (int32 i = 0; i < MAX_SENSORS - 1; i++)
			{
				switch (SensorArray[i])
				{
				case SENSOR_RADAR:
					if (EnergyToSpendThisTurn >= RADAR_SENSOR_ENERGY_COST / 60 * DeltaTime)
					{
						EnergyToSpendThisTurn -= RADAR_SENSOR_ENERGY_COST / 60 * DeltaTime;
						//Turn on Sensor
					}
					else
					{
						//Turn off Sensor
					}
					break;
				case SENSOR_RANGE:
					if (EnergyToSpendThisTurn >= RANGE_SENSOR_ENERGY_COST / 60 * DeltaTime)
					{
						EnergyToSpendThisTurn -= RANGE_SENSOR_ENERGY_COST / 60 * DeltaTime;
						//Turn on Sensor
					}
					else
					{
						//Turn off Sensor
					}
					break;
				default:
					break;
				}
			}
			break;
		case SYSTEM_LASERS:
			//If the System is fully charged, don't try to give it Energy.
			if (!(CurrentEnergy[SYSTEM_LASERS] >= MAX_LASER_ENERGY))
			{
				if (EnergyToSpendThisTurn >= EnergyChargeRatePerSecond[SYSTEM_LASERS])
				{
					EnergyToSpendThisTurn -= EnergyChargeRatePerSecond[SYSTEM_LASERS];
					CurrentEnergy[SYSTEM_LASERS] += EnergyChargeRatePerSecond[SYSTEM_LASERS];
				}
				else
				{
					CurrentEnergy[SYSTEM_LASERS] += EnergyToSpendThisTurn;
					EnergyToSpendThisTurn = 0;
				}

				//Check for extra energy in system. If system is overcharged
				//return the extra energy to the pool of available energy this turn.
				if (CurrentEnergy[SYSTEM_LASERS] > MAX_SHIELD_ENERGY)
				{
					float ExtraEnergy = CurrentEnergy[SYSTEM_LASERS] - MAX_SHIELD_ENERGY;
					CurrentEnergy[SYSTEM_LASERS] = MAX_SHIELD_ENERGY;
					EnergyToSpendThisTurn += ExtraEnergy;
				}
			}
			break;
		case SYSTEM_MISSILES:
			//If the System is fully charged, don't try to give it Energy.
			if (!(CurrentEnergy[SYSTEM_MISSILES] >= MAX_LASER_ENERGY))
			{
				if (EnergyToSpendThisTurn >= EnergyChargeRatePerSecond[SYSTEM_MISSILES])
				{
					EnergyToSpendThisTurn -= EnergyChargeRatePerSecond[SYSTEM_MISSILES];
					CurrentEnergy[SYSTEM_MISSILES] += EnergyChargeRatePerSecond[SYSTEM_MISSILES];
				}
				else
				{
					CurrentEnergy[SYSTEM_MISSILES] += EnergyToSpendThisTurn;
					EnergyToSpendThisTurn = 0;
				}

				//Check for extra energy in system. If system is overcharged
				//return the extra energy to the pool of available energy this turn.
				if (CurrentEnergy[SYSTEM_MISSILES] > MAX_SHIELD_ENERGY)
				{
					float ExtraEnergy = CurrentEnergy[SYSTEM_MISSILES] - MAX_SHIELD_ENERGY;
					CurrentEnergy[SYSTEM_MISSILES] = MAX_SHIELD_ENERGY;
					EnergyToSpendThisTurn += ExtraEnergy;
				}
			}
			break;
		default:
			break;
		}
	}
}

