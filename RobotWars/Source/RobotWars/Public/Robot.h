// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Missile.h"
#include "Robot.generated.h"

#define MAX_TREAD_SPEED 100.0f
#define MIN_THREAH_SPEED -100.0f
#define MAX_SPEED 100.0f				//In cm/s
#define TREAD_DISTANCE 15.0f

UCLASS()
class ROBOTWARS_API ARobot : public APawn
{
	GENERATED_BODY()

///********** METHOD ***********************************************************************************************************************************************
public:
	// Sets default values for this pawn's properties
	ARobot();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void SetRobotColor(FLinearColor color);
	void GetHit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void SetRobotName(FString RobotNewName);
	void SetTreadSpeed(float LeftThread, float RightThread);
	
	bool FireMissile();

private:
	void MoveRobot(float DeltaTime);
	void UpdateSensor();

///********** VARIABLE **************************************************************************************************************************************************

public:
	FString RobotName = TEXT("DEFAULT");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
	//UPROPERTY(EditAnywhere)
		TSubclassOf<AMissile> MissileToSpawn;

protected:

	// Debug tool - Show which way the Robot is facing
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		class UArrowComponent* RobotDirection;

	//Sprite for the Robot.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		class UPaperSpriteComponent* RobotSprite;


private:
	int32 LeftTreadSpeed = 0;
	int32 RightTreadSpeed = 0;

	class UMaterial* ShieldMaterialHelper;
	class UMaterialInstanceDynamic* ShieldMaterial;
	FLinearColor RobotColor;

	//The Robot collision capsule
	UPROPERTY(VisibleAnywhere, Category = "Robot")
		class UCapsuleComponent* RobotCollisionCapsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* RobotShield;

	//The in Development Camera - Should be remove and put in an observer in final version
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* CameraComponent;
	
	//The in Development Springarm - Should be remove and put in an observer in final version
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* SpringArm;

	/* TEST VARIALBE*/

};
