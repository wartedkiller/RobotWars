// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "GameFramework/Actor.h"
#include "Laser.generated.h"

#define LASER_SPEED		200.0f

UCLASS()
class ROBOTWARS_API ALaser : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALaser();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SetDamage(int32 Damage);

private:
	void Explode();
	

	/********VARIABLES********/

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
		FName MovementCollisionProfile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
		float Radius;

protected:
	// Debug tool - Show which way the Laser is facing
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Laser", meta = (AllowPrivateAccess = "true"))
		class UArrowComponent* LaserDirection;

	//Sprite for the Laser.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Laser", meta = (AllowPrivateAccess = "true"))
		class UPaperSpriteComponent* LaserSprite;

private:
	int32 LaserDamage = 0;
};
