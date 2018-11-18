// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Missile.h"
#include "ExplosionActor.h"
#include "RobotWarsEnum.h"
#include "SensorSystem.h"
#include "TimerManager.h"
#include "RobotWarsStatics.h"
#include "Robot.generated.h"


UCLASS()
class ROBOTWARS_API ARobot : public AActor
{
	GENERATED_BODY()

///********** METHOD ***********************************************************************************************************************************************
public:
	// Sets default values for this pawn's properties
	ARobot();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Robot")
		void SetRobotColor(FLinearColor color);
	void GetHit(DAMAGETYPE DamageType, float DamageValue);
	USensorSystem* GetSensor(int32 index);

	float GetRobotSpeed();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Other methods
	void SetRobotName(FString RobotNewName);
	void SetStatusMessage(FString Message);
	//void AbortOnError(FString Message);
	float GetRandomNumber(int32 UpperBound);

	//Movement methods
	void SetMotorSpeeds(int32 LeftThread, int32 RightThread);
	int32 TurboBoost();
	int32 IsTurboOn();
	
	//WeaponSystem methods
	void FireWeapon(WEAPONTYPE type, int32 heading);

	//EnergySystem methods
	int32 GetGeneratorStructure();
	int32 GetGeneratorOutput();
	int32 SetSystemChargePriorites(SYSTEM priorities[NUM_ENERGY_SYSTEMS]);
	float GetSystemEnergy(SYSTEM type);
	void SetSystemChargeRate(SYSTEM type, int32 rate);

	//Sensor method
	int32 AddSensor(int32 port, SENSORTYPE type, int32 angle, int32 width, int32 range);
	int32 GetSensorData(int32 port);
		//status might need to change for a bool.
	void SetSensorStatus(int32 port, int32 status);
	int32 GetBumpInfo();
	GPS_INFO GetGPSInfo();

private:
	void MoveRobot(float DeltaTime);
	void UpdateSensor();
	void UpdateEnergy(float DeltaTime);
	void UpdateShield();
	void UpdateInformation();
	void KillThisRobot();
	void AddBumpInfo(DAMAGETYPE DamageType);

	UFUNCTION()
	void TurnBoosOff();

	UFUNCTION()
		void RadarOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
		void RadarOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


///********** VARIABLE **************************************************************************************************************************************************

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		FString RobotName = TEXT("DEFAULT");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		FString StatusMessage = TEXT("NO STATUS MESSAGE");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		float Score;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		float CurrentGeneratorCapacity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		float CurrentShieldCharge;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		float CurrentMissileCharge;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		float CurrentLaserCharge;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		float MaxGeneratorCapacity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		float MaxShieldCharge;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		float MaxMissileCharge;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		float MaxLaserCharge;

	UPROPERTY(VisibleAnywhere, Category = "Robot", meta = (AllowPrivateAccess = "true"))
	USensorSystem* SensorArray[MAX_SENSORS];

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Audio")
		class USoundBase* RobotExplosionAudioCue;

	// Debug tool - Show which way the Robot is facing
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		class UArrowComponent* RobotDirection;

	//Sprite for the Robot.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* RobotMesh;


private:
	int32 LeftTreadSpeed = 0;
	int32 RightTreadSpeed = 0;
	int32 TurboOn = 0;
	int32 BumpInfo = 0;

	float RobotSpeed = 0.0f;
	float RecoilVelocity = 0.0f;
	float RecoilHeading = 0.0f;
	bool bCollisionThisTurn = false;
	ARobot* PreviousFrameCollisionRobot = nullptr;

	FTimerHandle TurboTimerHandle;

	FName RobotCollisionProfile;

	class UMaterial* ShieldMaterialHelper;
	class UMaterialInstanceDynamic* ShieldMaterial;
	class UMaterial* RadarSensorMaterialHelper;
	class UMaterialInstanceDynamic* RadarSensorMaterial;
	class UMaterial* RangeSensorMaterialHelper;
	class UMaterialInstanceDynamic* RangeSensorMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		bool bIsAlive = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		FLinearColor RobotColor = FLinearColor::White;

	//The Robot collision capsule
	UPROPERTY(VisibleAnywhere, Category = "Robot")
		class UCapsuleComponent* RobotCollisionCapsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* RobotShield;

	UPROPERTY()
		class UEnergySystem* EnergySystem;

	UPROPERTY()
		class ULaserSystem* LaserSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		class UMissileSystem* MissileSystem;

	UPROPERTY(VisibleAnywhere, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* SensorMeshArray[MAX_SENSORS];

	UPROPERTY(VisibleAnywhere, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		UCapsuleComponent* SensorCollisionCapsule[MAX_SENSORS];

	UPROPERTY(VisibleAnywhere, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<AExplosionActor> ExplosionActor;

	/* TEST VARIALBE*/
};
