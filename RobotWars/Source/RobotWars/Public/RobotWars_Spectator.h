// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "RobotWars_Spectator.generated.h"

UCLASS()
class ROBOTWARS_API ARobotWars_Spectator : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ARobotWars_Spectator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
private:

	//The in Development Camera - Should be remove and put in an observer in final version
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spectator", meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* CameraComponent;

	//The in Development Springarm - Should be remove and put in an observer in final version
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spectator", meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* SpringArm;
	
};
