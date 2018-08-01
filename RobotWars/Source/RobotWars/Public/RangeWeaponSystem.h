// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RangeWeaponSystem.generated.h"

UCLASS()
class ROBOTWARS_API ARangeWeaponSystem : public AActor
{
	GENERATED_BODY()
	

/********METHODS********/
public:	
	// Sets default values for this actor's properties
	ARangeWeaponSystem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	int GetWeaponEnergy() const;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void FireWeapon(FVector StartPosition, float Direction) { check(0 && "You must override this"); }
	
/********VARIABLES********/
public:


protected:


private:
	int CurrentEnergy;
};
