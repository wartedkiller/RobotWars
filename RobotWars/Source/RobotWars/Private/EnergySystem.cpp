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

	CurrentEnergy[SYSTEM_SHIELDS] = 400;

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

int32 UEnergySystem::GetLaserDamage()
{
	if (CurrentEnergy[SYSTEM_LASERS] >= MIN_LASER_ENERGY && CurrentEnergy[SYSTEM_LASERS] <= MAX_LASER_ENERGY)
	{
		int32 Damage = CurrentEnergy[SYSTEM_LASERS];
		CurrentEnergy[SYSTEM_LASERS] = 0;
		return Damage;
	}
	else
	{
		CurrentEnergy[SYSTEM_LASERS] = 0;
		return 0;
	}
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


void UEnergySystem::UpdateEnergySystem(float DeltaTime, ARobot* robot)
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
			if (CurrentEnergy[SYSTEM_SHIELDS] < MAX_SHIELD_ENERGY)
			{
				float EnergyToSpendOnShield = EnergyChargeRatePerSecond[SYSTEM_SHIELDS] * DeltaTime;
				if (EnergyToSpendThisTurn >= EnergyToSpendOnShield)
				{
					EnergyToSpendThisTurn -= EnergyToSpendOnShield;
					CurrentEnergy[SYSTEM_SHIELDS] += EnergyToSpendOnShield;
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
			for (int32 i = 0; i < MAX_SENSORS; i++)
			{
				USensorSystem* CurrentSensor = robot->GetSensor(i);
				if (CurrentSensor->IsSensorOn())
				{
					switch (CurrentSensor->GetTypeOfSensor())
					{
					case SENSOR_RADAR:
						if (EnergyToSpendThisTurn >= RADAR_SENSOR_ENERGY_COST / 60 * DeltaTime)
						{
							EnergyToSpendThisTurn -= RADAR_SENSOR_ENERGY_COST / 60 * DeltaTime;
							CurrentSensor->SetIsEnoughEnergy(true);
						}
						else
						{
							CurrentSensor->SetIsEnoughEnergy(false);
						}
						break;
					case SENSOR_RANGE:
						if (EnergyToSpendThisTurn >= RANGE_SENSOR_ENERGY_COST / 60 * DeltaTime)
						{
							EnergyToSpendThisTurn -= RANGE_SENSOR_ENERGY_COST / 60 * DeltaTime;
							CurrentSensor->SetIsEnoughEnergy(true);
						}
						else
						{
							CurrentSensor->SetIsEnoughEnergy(false);
						}
						break;
					default:
						break;
					}
				}
			}
			break;
		case SYSTEM_LASERS:
			//If the System is fully charged, don't try to give it Energy.
			if (CurrentEnergy[SYSTEM_LASERS] < MAX_LASER_ENERGY)
			{
				float EnergyToSpendOnLaser = EnergyChargeRatePerSecond[SYSTEM_LASERS] * DeltaTime;

				if (EnergyToSpendThisTurn >= EnergyToSpendOnLaser)
				{
					EnergyToSpendThisTurn -= EnergyToSpendOnLaser;
					CurrentEnergy[SYSTEM_LASERS] += EnergyToSpendOnLaser;
				}
				else
				{
					CurrentEnergy[SYSTEM_LASERS] += EnergyToSpendThisTurn;
					EnergyToSpendThisTurn = 0;
				}

				//Check for extra energy in system. If system is overcharged
				//return the extra energy to the pool of available energy this turn.
				if (CurrentEnergy[SYSTEM_LASERS] > MAX_LASER_ENERGY)
				{
					float ExtraEnergy = CurrentEnergy[SYSTEM_LASERS] - MAX_LASER_ENERGY;
					CurrentEnergy[SYSTEM_LASERS] = MAX_LASER_ENERGY;
					EnergyToSpendThisTurn += ExtraEnergy;
				}
			}
			break;
		case SYSTEM_MISSILES:
			//If the System is fully charged, don't try to give it Energy.
			if (CurrentEnergy[SYSTEM_MISSILES] < MAX_MISSILE_ENERGY)
			{
				float EnergyToSpendOnMissile = EnergyChargeRatePerSecond[SYSTEM_MISSILES] * DeltaTime;
				if (EnergyToSpendThisTurn >= EnergyToSpendOnMissile)
				{
					EnergyToSpendThisTurn -= EnergyToSpendOnMissile;
					CurrentEnergy[SYSTEM_MISSILES] += EnergyToSpendOnMissile;
				}
				else
				{
					CurrentEnergy[SYSTEM_MISSILES] += EnergyToSpendThisTurn;
					EnergyToSpendThisTurn = 0;
				}

				//Check for extra energy in system. If system is overcharged
				//return the extra energy to the pool of available energy this turn.
				if (CurrentEnergy[SYSTEM_MISSILES] > MAX_MISSILE_ENERGY)
				{
					float ExtraEnergy = CurrentEnergy[SYSTEM_MISSILES] - MAX_MISSILE_ENERGY;
					CurrentEnergy[SYSTEM_MISSILES] = MAX_MISSILE_ENERGY;
					EnergyToSpendThisTurn += ExtraEnergy;
				}
			}
			break;
		default:
			break;
		}
	}
}

bool UEnergySystem::RemoveEnergy(int32 Value)
{
	if (CurrentEnergy[SYSTEM_SHIELDS] >= Value)
	{
		CurrentEnergy[SYSTEM_SHIELDS] -= Value;
	}
	else
	{
		Value -= CurrentEnergy[SYSTEM_SHIELDS];
		CurrentEnergy[SYSTEM_SHIELDS] = 0;
		CurrentGeneratorStructure -= Value;

		if (CurrentGeneratorStructure <= 0)
		{
			return false;
		}
	}

	return true;
}

