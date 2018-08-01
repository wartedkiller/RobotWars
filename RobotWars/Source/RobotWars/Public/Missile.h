// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Missile.generated.h"

UCLASS()
class ROBOTWARS_API AMissile : public AActor
{
	GENERATED_BODY()
	
/********METHODS********/
public:	
	// Sets default values for this actor's properties
	AMissile();
	void SetInitialCondition(FVector InitialPosition, float Direction);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Debug tool - Show which way the Robot is facing
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		class UArrowComponent* MissileDirection;

	//Sprite for the Robot.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		class UPaperSpriteComponent* MissileSprite;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


/********VARIABLES********/
	
};
