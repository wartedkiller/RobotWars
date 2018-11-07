// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "GameFramework/Actor.h"
#include "Missile.generated.h"

#define MISSILE_SPEED				150.0f
#define MISSILE_DAMAGE				150
#define MISSILE_EXPLOSION_RADIUS	50.0f
#define MISSILE_SPLASH_DAMAGE		50.0f

UCLASS()
class ROBOTWARS_API AMissile : public AActor
{
	GENERATED_BODY()
	
/********METHODS********/
public:	
	// Sets default values for this actor's properties
	AMissile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Debug tool - Show which way the Missile is facing
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Missile", meta = (AllowPrivateAccess = "true"))
		class UArrowComponent* MissileDirection;

	//Sprite for the Missile.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Missile", meta = (AllowPrivateAccess = "true"))
		class UPaperSpriteComponent* MissileSprite;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void Explode(FVector ExplosionLocation);


/********VARIABLES********/

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
		FName MovementCollisionProfile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
		float Radius;

private:

	class AActor* ActorHitByMissile = nullptr;
	
};
