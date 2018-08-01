// Fill out your copyright notice in the Description page of Project Settings.

#include "RangeWeaponSystem.h"


// Sets default values
ARangeWeaponSystem::ARangeWeaponSystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARangeWeaponSystem::BeginPlay()
{
	Super::BeginPlay();
	
}

int ARangeWeaponSystem::GetWeaponEnergy() const
{
	return CurrentEnergy;
}

// Called every frame
void ARangeWeaponSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

