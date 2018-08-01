// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RangeWeaponSystem.h"
#include "Missile.h"
#include "MissileSystem.generated.h"

/**
 * 
 */
UCLASS()
class ROBOTWARS_API AMissileSystem : public ARangeWeaponSystem
{
	GENERATED_BODY()
	
	
public:
	void FireWeapon(FVector StartPosition, float Direction) override;
	
};
