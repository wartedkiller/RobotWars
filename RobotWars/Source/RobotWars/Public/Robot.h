// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Missile.h"
#include "RobotWarsEnum.h"
#include "SensorSystem.h"
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
	USensorSystem* GetSensor(int index);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetRobotName(FString RobotNewName);
	void SetTreadSpeed(float LeftThread, float RightThread);
	
	//WeaponSystem methods
	void FireMissile();

	//EnergySystem methods
	int32 GetGeneratorStructur();
	int32 GetGeneratorOutput();
	int32 SetSystemChargePriorites(SYSTEM priorities[NUM_ENERGY_SYSTEMS]);
	float GetSystemEnergy(SYSTEM type);
	void SetSystemChargeRate(SYSTEM type, int32 rate);

	//Sensor method
	int32 AddSensor(int port, SENSORTYPE type, int angle, int width, int range);

private:
	void MoveRobot(float DeltaTime);
	void UpdateSensor();
	void UpdateEnergy(float DeltaTime);

///********** VARIABLE **************************************************************************************************************************************************

public:
	FString RobotName = TEXT("DEFAULT");

	UPROPERTY(VisibleAnywhere, Category = "Robot", meta = (AllowPrivateAccess = "true"))
	USensorSystem* SensorArray[MAX_SENSORS];

protected:

	// Debug tool - Show which way the Robot is facing
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		class UArrowComponent* RobotDirection;

	//Sprite for the Robot.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* RobotMesh;


private:
	int32 LeftTreadSpeed = 0;
	int32 RightTreadSpeed = 0;

	class UMaterial* ShieldMaterialHelper;
	class UMaterialInstanceDynamic* ShieldMaterial;
	class UMaterial* RadarSensorMaterialHelper;
	class UMaterialInstanceDynamic* RadarSensorMaterial;
	class UMaterial* RangeSensorMaterialHelper;
	class UMaterialInstanceDynamic* RangeSensorMaterial;
	FLinearColor RobotColor;

	//The Robot collision capsule
	UPROPERTY(VisibleAnywhere, Category = "Robot")
		class UCapsuleComponent* RobotCollisionCapsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* RobotShield;

	UPROPERTY()
		class UEnergySystem* EnergySystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		class UMissileSystem* MissileSystem;

	//The in Development Camera - Should be remove and put in an observer in final version
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* CameraComponent;
	
	//The in Development Springarm - Should be remove and put in an observer in final version
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* SpringArm;

	UPROPERTY()
		UStaticMeshComponent* SensorMeshArray[MAX_SENSORS];

	/* TEST VARIALBE*/

};
