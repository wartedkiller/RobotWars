// Fill out your copyright notice in the Description page of Project Settings.

#include "Robot.h"
#include "PaperSpriteComponent.h"
#include "Components/ArrowComponent.h"
#include "Camera/CameraComponent.h"
#include "RobotWarsStatics.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "Components/LineBatchComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/StaticMesh.h"
#include "MissileSystem.h"
#include "LaserSystem.h"
#include "EnergySystem.h"
#include "SensorSystem.h"
#include "DrawDebugHelpers.h"


/***********************************************************************************************

Mehtod:			ARobot

Description:	This is where the Robot is initialized. This method is called before the Engine
				is fully initialized and it creates and initialize every static mesh for the
				Robot as well as each system.

Parameters:		Nothing.

Returns:		Nothing.

Note: IF ANYTHING CHANGE IN THE CONSTRUCTOR OF THIS CLASS, THE ENGINE MUST BE RESTARTED TO SEE
THE CHANGE. FAILING TO DO SO WILL RESULT IN SEEING WEIRD RESULT OF WHAT YOU ARE TRYING TO DO OR 
THE ENGINE WILL SIMPLY CRASH.
***********************************************************************************************/
ARobot::ARobot()
{
 	// Set this pawn to call Tick() every frame except when the game is paused and before BeginPlay() method is called. 
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bTickEvenWhenPaused = false;
	bAllowTickBeforeBeginPlay = false;

	//Creating a USceneComponent as the RootComponent of the Robot since
	//USceneComponent are the lightest object that keep track of it's position.
	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RobotBase"));
	}

	//Creating the Robot debug UArrowComponent. It will not render in the final game
	//but it will still help keeping track of the direction of the Robot.
	RobotDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("RobotDirection"));
	RobotDirection->SetupAttachment(RootComponent);
	
	//Creating the default Robot sprite.
	//NOTE: Paper2D is an abandon ware in Unreal 4. While it will always be
	//in the Engine, it won't receive future upgrade. A solution to this is
	//using a 3D plane like it's done for the shield.
	RobotMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RobotMesh"));
	RobotMesh->SetupAttachment(RobotDirection);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> RobotVisualAsset(TEXT("StaticMesh'/Game/Mesh/Robot.Robot'"));
	if (RobotVisualAsset.Succeeded())
	{
		RobotMesh->SetStaticMesh(RobotVisualAsset.Object);
		RobotMesh->SetWorldLocation(RobotDirection->GetComponentLocation() + FVector(0.0f, 0.0f, -1.0f));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not load static mesh for the Robot"))
	}

	//Creating the UCapsuleComponent used for collision. Since it's a 2D game, the Z position (height)
	//of the Robots is not important. That's why the Capsule is 200 unit high.
	RobotCollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RobotCollisionCapsule"));
	RobotCollisionCapsule->SetupAttachment(RobotDirection);
	RobotCollisionCapsule->InitCapsuleSize(25.0f, 200.0f); 

	//Creating the UStaticMeshComponent for the Shield.
	RobotShield = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RobotShield"));
	RobotShield->SetupAttachment(RobotDirection);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShieldVisualAsset(TEXT("StaticMesh'/Game/Mesh/Shield.Shield'"));
	if (ShieldVisualAsset.Succeeded())
	{
		RobotShield->SetStaticMesh(ShieldVisualAsset.Object);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not load static mesh for the Shield"))
	}

	//Loading the materials to be used in the BeginPlay() method.
	static ConstructorHelpers::FObjectFinder<UMaterial> ShieldMaterialGetter(TEXT("Material'/Game/Material/ShieldMaterial.ShieldMaterial'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> RangeSensorMaterialGetter(TEXT("Material'/Game/Material/RangeSensorMaterial_MAT.RangeSensorMaterial_MAT'"));
	if (ShieldMaterialGetter.Succeeded() && RangeSensorMaterialGetter.Succeeded())
	{
		ShieldMaterialHelper = ShieldMaterialGetter.Object;
		RadarSensorMaterialHelper = ShieldMaterialGetter.Object;
		RangeSensorMaterialHelper = RangeSensorMaterialGetter.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not load the Shield material."))
	}

	//Initializing Sensors
	SensorMeshArray[0] = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SensorMesh0"));
	SensorMeshArray[0]->SetupAttachment(RobotDirection);
	SensorMeshArray[1] = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SensorMesh1"));
	SensorMeshArray[1]->SetupAttachment(RobotDirection);
	SensorMeshArray[2] = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SensorMesh2"));
	SensorMeshArray[2]->SetupAttachment(RobotDirection);
	SensorMeshArray[3] = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SensorMesh3"));
	SensorMeshArray[3]->SetupAttachment(RobotDirection);

	RobotCollisionProfile = TEXT("Robot");

}

/***********************************************************************************************

Mehtod:			BeginPlay

Description:	This method is called by the Engine right after the constructor. It initialized
				what couldn't be initialized in the constructor due to the Engine not behing
				fully initialized.

Parameters:		Nothing.

Returns:		Nothing.

Note:			Nothing.
***********************************************************************************************/
void ARobot::BeginPlay()
{
	Super::BeginPlay();

	if (ShieldMaterialHelper)
	{
		ShieldMaterial = RobotShield->CreateDynamicMaterialInstance(0, ShieldMaterialHelper);
		RobotShield->SetMaterial(0, ShieldMaterial);
		RobotShield->SetWorldScale3D(FVector(1.0f));

		ShieldMaterial->SetVectorParameterValue("ShieldColor", RobotColor);
	}
	if (RadarSensorMaterialHelper)
	{
		RadarSensorMaterial = SensorMeshArray[0]->CreateDynamicMaterialInstance(0, RadarSensorMaterialHelper);
		RadarSensorMaterial->SetVectorParameterValue("ShieldColor", RobotColor);
	}
	if (RangeSensorMaterialHelper)
	{
		RangeSensorMaterial = SensorMeshArray[0]->CreateDynamicMaterialInstance(0, RangeSensorMaterialHelper);
		RangeSensorMaterial->SetVectorParameterValue("SensorColor", RobotColor);
	}
	
	//Initialize all Sensor to SENSOR_NONE.
	for (int32 i = 0; i < MAX_SENSORS; i++)
	{
		SensorArray[i] = NewObject<USensorSystem>(this, USensorSystem::StaticClass());
		SensorArray[i]->RegisterComponent();
		SensorArray[i]->SetWorldLocation(RobotDirection->GetComponentLocation());
		SensorArray[i]->AttachToComponent(RobotDirection, FAttachmentTransformRules::KeepWorldTransform);
	}

	//This is a test to try and solve the problem with the MissileSystem.
	MissileSystem = NewObject<UMissileSystem>(this, UMissileSystem::StaticClass());

	LaserSystem = NewObject<ULaserSystem>(this, ULaserSystem::StaticClass());

	EnergySystem = NewObject<UEnergySystem>(this, UEnergySystem::StaticClass());

	MaxGeneratorCapacity = MAX_GENERATOR_STRUCTURE;
	MaxShieldCharge = MAX_SHIELD_ENERGY;
	MaxLaserCharge = MAX_LASER_ENERGY;
	MaxMissileCharge = MAX_MISSILE_ENERGY;
}

/***********************************************************************************************

Mehtod:			SetRobotName

Description:	This method set the name of the Robot. Student will call this in their BeginPlay
				method.

Parameters:		FString RobotNewName	-The name of the Robot.

Returns:		Nothing.

Note:			Nothing.
***********************************************************************************************/
void ARobot::SetRobotName(FString RobotNewName)
{
	RobotName = RobotNewName;
}

/***********************************************************************************************

Mehtod:			SetStatusMessage

Description:	This method set the status message displayed on the UI for this Robot.

Parameters:		char* Message	-The message to be displayed on the UI.

Returns:		Nothing.

Note:			Nothing.
***********************************************************************************************/
void ARobot::SetStatusMessage(FString Message)
{
	StatusMessage =  Message;
}

/***********************************************************************************************

Mehtod:			GetRandomNumber

Description:	This method set the status message displayed on the UI for this Robot.

Parameters:		int32 UpperBound

Returns:		float	-Random float between 0 and UpperBound.

Note:			Nothing.
***********************************************************************************************/
float ARobot::GetRandomNumber(int32 UpperBound)
{
	return FMath::RandRange(0, UpperBound);
}

/***********************************************************************************************

Mehtod:			SetMotorSpeeds

Description:	This method set the speed of each Tread.

Parameters:		int32 LeftTread		-The speed of the left tread
				int32 RightTread	-The speed of the right tread

Returns:		Nothing.

Note:			Nothing.
***********************************************************************************************/
void ARobot::SetMotorSpeeds(int32 LeftTread, int32 RightTread)
{
	if (LeftTread < MIN_TREAD_SPEED)
	{
		LeftTread = MIN_TREAD_SPEED;
	}
	else if (LeftTread > MAX_TREAD_SPEED)
	{
		LeftTread = MAX_TREAD_SPEED;
	}

	if (RightTread < MIN_TREAD_SPEED)
	{
		RightTread = MIN_TREAD_SPEED;
	}
	else if (RightTread > MAX_TREAD_SPEED)
	{
		RightTread = MAX_TREAD_SPEED;
	}
	

	LeftTreadSpeed = LeftTread;
	RightTreadSpeed = RightTread;
}

/***********************************************************************************************

Mehtod:			TurboBoost

Description:	This method activate the boost. It set a timer to call the TurnBoosOff method
				after TURBO_TIME secondes.

Parameters:		Nothing.

Returns:		int32	- 1 if the Turbo has been turned on correctly.
						- 0 if the Turbo hasn't been turned on.

Note:			Nothing.
***********************************************************************************************/
int32 ARobot::TurboBoost()
{
	if (EnergySystem->GetSystemEnergy(SYSTEM_SHIELDS) >= TURBO_ENERGY_COST)
	{
		EnergySystem->RemoveEnergy(TURBO_ENERGY_COST);

		GetWorldTimerManager().SetTimer(TurboTimerHandle, this, &ARobot::TurnBoosOff, TURBO_TIME);
		TurboOn = 1;
		return 1;
	}
	return 0;
}

/***********************************************************************************************

Mehtod:			IsTurboOn

Description:	This method return the status of the TurboBoost.

Parameters:		Nothing.

Returns:		int32	- 0 if Turbo is OFF
				int32	- 1 if Turbo is ON.

Note:			Nothing.
***********************************************************************************************/
int32 ARobot::IsTurboOn()
{
	return TurboOn;
}

/***********************************************************************************************

Mehtod:			FireWeapon

Description:	This method is called when a Student want to fire any kind of weapon. The
				heading is based on the forward direction of the Robot.

Parameters:		WEAPONTYPE type		- Which weapon do you want to fire.
				int32 heading		- The direction at which the projectile will be fired.

Returns:		Nothing.

Note:			Nothing.
***********************************************************************************************/
void ARobot::FireWeapon(WEAPONTYPE type, int32 heading)
{
	float Damage = 0;
	heading = heading % 360;

	switch (type)
	{
	case WEAPON_MISSILE:
		if ((Damage = EnergySystem->GetWeaponDamage(WEAPON_MISSILE)) > 0)
		{
			if (heading >= -MISSILE_MAX_ANGLE && heading <= MISSILE_MAX_ANGLE)
			{
				FRotator MissileHeading = RobotDirection->GetComponentRotation();
				MissileHeading.Yaw += heading;
				MissileSystem->Fire(this, GetActorLocation(), MissileHeading, Damage);
			}
		}
		break;
	case WEAPON_LASER:
		if ((Damage = EnergySystem->GetWeaponDamage(WEAPON_LASER)) > 0)
		{
				FRotator LaserHeading = RobotDirection->GetComponentRotation();
			LaserHeading.Yaw += heading;
			LaserSystem->Fire(this, GetActorLocation(), LaserHeading, Damage);
		}
		break;
	default:
		break;
	}
}

/***********************************************************************************************

Mehtod:			GetGeneratorStructure

Description:	This method get the remaining Structure Point from the Generator of the Robot.

Parameters:		Nothing.

Returns:		int32	- The remaining Structure Point

Note:			Nothing.
***********************************************************************************************/
int32 ARobot::GetGeneratorStructure()
{
	return EnergySystem->GetGeneratorStructure();
}

/***********************************************************************************************

Mehtod:			GetGeneratorOutput

Description:	This method get the Energy Point generated by the Generator each minute.

Parameters:		Nothing.

Returns:		int32	- The number of Energy Point generated.

Note:			Nothing.

//TODO Check the int32-float conversion.
***********************************************************************************************/
int32 ARobot::GetGeneratorOutput()
{
	return EnergySystem->GetGeneratorOutput();
}

/***********************************************************************************************

Mehtod:			SetSystemChargePriorites

Description:	This method set the priority of the different system to be alocated Energy Point
				each turn. The first element is the top priority. It can be change at any moment
				by the Student.

Parameters:		SYSTEM priorities[NUM_ENERGY_SYSTEMS]	- An array of the different system.
														
Returns:		int32	- 1 if the assignation of the SYSTEM has been made correctly.
						(Right size, no duplicate, ...)
						- 0 if there is an error with the assignation.

Note:			Nothing.
***********************************************************************************************/
int32 ARobot::SetSystemChargePriorites(SYSTEM priorities[NUM_ENERGY_SYSTEMS])
{
	return EnergySystem->SetSystemChargePriorites(priorities);
}

/***********************************************************************************************

Mehtod:			GetSystemEnergy

Description:	This method get the Energy Point stored by the desired SYSTEM.

Parameters:		SYSTEM type		- The SYSTEM to get the Energy Point.

Returns:		float	- The Energy Point stored in the SYSTEM.

Note:			Nothing.
***********************************************************************************************/
float ARobot::GetSystemEnergy(SYSTEM type)
{
	return EnergySystem->GetSystemEnergy(type);
}

/***********************************************************************************************

Mehtod:			SetSystemChargeRate

Description:	This method set the max number of Energy Point to be assigned to the specified 
				SYSTEM.

Parameters:		SYSTEM type		- The SYSTEM to set the Energy Point charge rate.
				int32 rate		- The rate at which the SYSTEM will be charged.

Returns:		Nothing.

Note:			Nothing.
***********************************************************************************************/
void ARobot::SetSystemChargeRate(SYSTEM type, int32 rate)
{
	EnergySystem->SetSystemChargeRate(type, rate);
}

/***********************************************************************************************

Mehtod:			AddSensor

Description:	This method add a Sensor to the Robot. The Student can set the angle, the width
				and the range of the sensor. The method load a StaticMesh into an already
				initialized StaticMeshComponent. The default StaticMesh is the biggest size
				it the Sensor can be and it's scalled down depending on the parameter.

Parameters:		int32 port			- The port of the Sensor.
				SENSORTYPE type		- The type of Sensor to be installed on the Robot.
				int32 angle			- The azimuth of the Sensor. 0 is forward.
				int32 width			- The width of the RADAR. No effect for the RANGE Sensor.
				int32 range			- The maximum range of the Sensor.

Returns:		int32		- 1 if the Sensor has been add correctly to the Robot.
							- 0 if there was an error while installing the Sensor.

Note:			- Right now Sensors are UStaticMeshComponent but in a futur version might
				become AActor. AActor are easier to work with since now I have to keep an
				SensorSystem array and a SensorMeshArray to make it work. There is also a
				bug in the Engine where I can't render a UStaticMeshComponent from an
				outside class on the Robot. This is why I have 2 array. Should be fixed
				with Engine version 4.22 but the bug shouldn't mater after the change to
				AActor.
***********************************************************************************************/
int32 ARobot::AddSensor(int32 port, SENSORTYPE type, int32 angle, int32 width, int32 range)
{
	float TrueWidth = width;
	float TrueRange = range;

	if (type == SENSOR_RADAR)
	{
		if (width < MIN_RADAR_ARC)
		{
			TrueWidth = MIN_RADAR_ARC;
		}
		else if (width > MAX_RADAR_ARC)
		{
			TrueWidth = MAX_RADAR_ARC;
		}

		if (range < RADAR_MIN_RANGE)
		{
			TrueRange = RADAR_MIN_RANGE;
		}
		else if (range > RADAR_MAX_RANGE)
		{
			TrueRange = RADAR_MAX_RANGE;
		}
	}
	else if (type == SENSOR_RANGE)
	{
		if (range > RANGE_MAX_RANGE)
		{
			range = RANGE_MAX_RANGE;
		}
	}

	if (port < MAX_SENSORS)
	{
		//A temporary check to fake that you can't change your sensor. In the end version, the game state will be chekced
		if (SensorArray[port]->GetTypeOfSensor() == SENSOR_NONE)
		{
			if (type == SENSOR_RADAR)
			{
				FString MeshPath = TEXT("StaticMesh'/Game/Mesh/RadarSensor.RadarSensor'");
				SensorMeshArray[port]->SetStaticMesh(Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *MeshPath)));
				SensorMeshArray[port]->SetWorldRotation(FRotator(0.0f, RobotDirection->GetComponentRotation().Yaw + angle, 0.0f));
				SensorMeshArray[port]->SetWorldScale3D(FVector(TrueRange / RADAR_MAX_RANGE, TrueWidth / MAX_RADAR_ARC, 1.0f));
				SensorMeshArray[port]->SetCollisionProfileName(TEXT("RadarSensor"));
				SensorMeshArray[port]->OnComponentBeginOverlap.AddDynamic(this, &ARobot::RadarOverlap);
				SensorMeshArray[port]->OnComponentEndOverlap.AddDynamic(this, &ARobot::RadarOverlapEnd);
				SensorMeshArray[port]->SetMaterial(0, RadarSensorMaterial);
				return SensorArray[port]->AddSensor(type, angle, TrueWidth, TrueRange);
			}
			else
			{
				//All the collision and the scalling of the sensor is made in the UpdateSensor() method.
				FString MeshPath = TEXT("StaticMesh'/Game/Mesh/RangeSensor_Circle.RangeSensor_Circle'");
				SensorMeshArray[port]->SetStaticMesh(Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *MeshPath)));
				SensorMeshArray[port]->SetWorldScale3D(FVector(2.0f));
				SensorMeshArray[port]->SetMaterial(0, RangeSensorMaterial);
				return SensorArray[port]->AddSensor(type, angle, TrueWidth, TrueRange);
			}
		}
	}
	return 0;
}

/***********************************************************************************************

Mehtod:			GetSensorData

Description:	This method get the data from the Sensor.

Parameters:		int32 port		- The port to get the data.

Returns:		int32		RADAR	- 1 if a Robot is in the RADAR Sensor.
									- 0 if there's no Robot in the RADAR.
							RANGE	- The distance in UnrealUnit between the middle of the Robot
									and the end of the Sensor.

							- -1 if there is no Sensor at the port.

Note:			Nothing.
***********************************************************************************************/
int32 ARobot::GetSensorData(int32 port)
{
	if (SensorArray[port])
	{
		return SensorArray[port]->GetSensorData();
	}
	else
	{
		return -1;
	}
}

/***********************************************************************************************

Mehtod:			SetSensorStatus

Description:	This method turn ON or OFF the Sesnor at the desired port.

Parameters:		int32 port		- Port to change status.
				int32 status	- 1 to turn the Sensor ON.
								- 0 to turn the Sensor OFF.

Returns:		Nothing.

Note:			Nothing.
***********************************************************************************************/
void ARobot::SetSensorStatus(int32 port, int32 status)
{
	if (SensorArray[port])
	{
		if (status == 1)
		{
			SensorArray[port]->SetSensorStatus(true);
		}
		else if (status == 0)
		{
			SensorArray[port]->SetSensorStatus(false);
		}
	}
}

/***********************************************************************************************

Mehtod:			GetBumpInfo

Description:	This method get the Bumb information since last asked and return it to the Student.

Parameters:		Nothing.

Returns:		int32		- An int32 of the bumb information
							BUMP_WALL			0x01
							BUMP_ROBOT			0x02
							BUMP_MISSILE		0x04
							BUMP_LASER			0x08

Note:			Nothing.
***********************************************************************************************/
int32 ARobot::GetBumpInfo()
{
	int32 temp = BumpInfo;
	BumpInfo = 0;
	return temp;
}

/***********************************************************************************************

Mehtod:			GetGPSInfo

Description:	This method give the position of the Robot in the arena. the center of the
				arena is the (0, 0) point.

Parameters:		Nothing.

Returns:		GPS_INFO	- The (X, Y) coordinate of the Robot on the arena and the azimuth
							it's facing.
							- Return all 0's if there is not enough Energy Point to use the GPS.

Note:			Nothing.
***********************************************************************************************/
GPS_INFO ARobot::GetGPSInfo()
{
	FVector temp = GetActorLocation();
	GPS_INFO GPSInfo;

	if (EnergySystem->GetSystemEnergy(SYSTEM_SHIELDS) >= GPS_ENERGY_COST)
	{
		//Checking if there is enough energy in the shield before removing it because it will remove the cost in the Generator if there is not enough shield energy.
		EnergySystem->RemoveEnergy(GPS_ENERGY_COST);

		GPSInfo.x = temp.X;
		GPSInfo.y = temp.Y;
		GPSInfo.heading = GetActorRotation().Yaw;
	}
	else
	{
		GPSInfo.x = 0;
		GPSInfo.y = 0;
		GPSInfo.heading = 0;
	}

	return GPSInfo;
}

/***********************************************************************************************

Mehtod:			MoveRobot

Description:	This method move the Robot depending on the Treads speed set by the Student. It
				is executed in the following order.
					1- Check if there is a recoil to apply and applies it.
					2- Check if the TurboBoost is ON and add the speed if it is.
					3- Calculate the futur position and the futur heading of the Robot based on
						the treads speed.
					4- Check if the Robot will collide with another Robot, apply recoil and
						damage if it's the case.
					5- Move and turn the Robot.

Parameters:		float DeltaTime		- The number of seconds between THIS frame and the LAST frame.

Returns:		Nothing.

Note:			Nothing.

//TODO The Robot is turning super quickly when one of the thread is negative. Might want to slow it down.

***********************************************************************************************/
void ARobot::MoveRobot(float DeltaTime)
{
	//Initializing initial variable so they can be used even if they don't change.
	//The FuturHeading is initialized at the current rotation of the Robot.
	//The FuturPosition is initialized at the current position in the Arena.
	float FuturHeading = FMath::DegreesToRadians(RobotDirection->GetComponentRotation().Yaw);
	FVector FuturPosition = GetActorLocation();

	float DistanceLeftTread = 0.0f;
	float DistanceRightTread = 0.0f;

	//Check if there is a recoil happening from a Robot-Robot collision.
	if (RecoilVelocity > 0.0f)
	{
		FuturPosition += FVector(RecoilVelocity * DeltaTime * FMath::Cos(RecoilHeading), RecoilVelocity * DeltaTime * FMath::Sin(RecoilHeading), 0.0f);
		RecoilVelocity -= ARENA_FRICTION * DeltaTime;
		if (RecoilVelocity < 0.0f)
		{
			RecoilVelocity = 0.0f;
		}
	}

	//This is the lenght of the movement vector.
	//The direction (positive or negative) has been accounted for in the math.
	if (TurboOn)
	{
		DistanceLeftTread = ((FMath::Abs(LeftTreadSpeed) + TURBO_SPEED) / MAX_TREAD_SPEED) * MAX_SPEED * DeltaTime;
		DistanceRightTread = ((FMath::Abs(RightTreadSpeed) + TURBO_SPEED) / MAX_TREAD_SPEED) * MAX_SPEED * DeltaTime;
	}
	else
	{
		DistanceLeftTread = (FMath::Abs(LeftTreadSpeed) / MAX_TREAD_SPEED) * MAX_SPEED * DeltaTime;
		DistanceRightTread = (FMath::Abs(RightTreadSpeed) / MAX_TREAD_SPEED) * MAX_SPEED * DeltaTime;
	}


	//Case 1 : The Robot is moving without turning.
	//There is no math for this case since it's trivial.
	if (LeftTreadSpeed == RightTreadSpeed)
	{
		FuturPosition += FVector(DistanceLeftTread * FMath::Sign(LeftTreadSpeed) * FMath::Cos(FuturHeading), DistanceRightTread * FMath::Sign(RightTreadSpeed) * FMath::Sin(FuturHeading), 0.0f);
	}
	//Case 2 : One of the tread is not moving therefore the pivot point is at the position of the immobile tread.
	///Case 2 RightTread
	else if (LeftTreadSpeed == 0)
	{
		//I substract the angle so the Robot turns left because the RightThreadSpeed > LeftTreadSpeed.
		FuturHeading -= FMath::Atan(DistanceRightTread * FMath::Sign(RightTreadSpeed) / TREAD_DISTANCE);
		
		float MovementFromCenter = DistanceRightTread * FMath::Sign(RightTreadSpeed) / 2;

		FuturPosition += FVector(MovementFromCenter * FMath::Cos(FuturHeading), MovementFromCenter * FMath::Sin(FuturHeading), 0.0f);

	}
	///Case 2 LeftTread
	else if (RightTreadSpeed == 0)
	{
		FuturHeading += FMath::Atan(DistanceLeftTread * FMath::Sign(LeftTreadSpeed) / TREAD_DISTANCE);

		float MovementFromCenter = DistanceLeftTread * FMath::Sign(LeftTreadSpeed) / 2;

		FuturPosition += FVector(MovementFromCenter * FMath::Cos(FuturHeading), MovementFromCenter * FMath::Sin(FuturHeading), 0.0f);
	}
	//The Robot is turning left
	else if (RightTreadSpeed > LeftTreadSpeed)
	{
		//Case 3 : The Robot is turning on itself. The pivot point is in the middile of the Robot.
		///Case 3 RightTread
		if (DistanceLeftTread == DistanceRightTread)
		{
			FuturHeading -= FMath::Atan(DistanceRightTread * FMath::Sign(RightTreadSpeed) / (TREAD_DISTANCE / 2));
		}
		//Case 4 : Left tread speed is positive so the pivot point is outside of the Robot.
		///Case 4 RightTread
		else if (LeftTreadSpeed > 0)
		{
			float RadiusLeft = TREAD_DISTANCE / ((DistanceRightTread * FMath::Sign(RightTreadSpeed) / DistanceLeftTread * FMath::Sign(LeftTreadSpeed)) - 1);
			float RadiusCenter = RadiusLeft + (TREAD_DISTANCE / 2);
			float MovementFromCenter = (DistanceLeftTread * FMath::Sign(LeftTreadSpeed) * RadiusCenter) / RadiusLeft;

			FuturHeading -= FMath::Atan(MovementFromCenter / RadiusCenter);
			FuturPosition += FVector(MovementFromCenter * FMath::Cos(FuturHeading), MovementFromCenter * FMath::Sin(FuturHeading), 0.0f);
		}
		//Case 5 : Left tread is negative so the pivot is inside the Robot.
		///Case 5 RightTread
		else
		{
			float RightThreadToRotationPoint = (TREAD_DISTANCE * DistanceRightTread * FMath::Sign(RightTreadSpeed)) / (DistanceRightTread * FMath::Sign(RightTreadSpeed) + FMath::Abs(DistanceLeftTread));
			float CenterToRotationPoint = RightThreadToRotationPoint - (TREAD_DISTANCE / 2);

			float MovementFromCenter = (DistanceRightTread * FMath::Sign(RightTreadSpeed) / RightThreadToRotationPoint) * CenterToRotationPoint;

			FuturHeading -= FMath::Atan(MovementFromCenter / CenterToRotationPoint);
			FuturPosition += FVector(MovementFromCenter * FMath::Cos(FuturHeading), MovementFromCenter * FMath::Sin(FuturHeading), 0.0f);
		}
	}
	//The Robot is turning Right
	else if (LeftTreadSpeed > RightTreadSpeed)
	{
		if (DistanceLeftTread == DistanceRightTread)
		{
			FuturHeading += FMath::Atan(DistanceRightTread * FMath::Sign(RightTreadSpeed) / (TREAD_DISTANCE / 2));
		}
		//Pivot point is outside of the robot which mean wide turn
		else if (RightTreadSpeed > 0)
		{
			float RadiusRight = 1 / ((1 / TREAD_DISTANCE) * ((DistanceRightTread * FMath::Sign(RightTreadSpeed) / DistanceLeftTread * FMath::Sign(LeftTreadSpeed)) - 1));

			FuturHeading -= FMath::Atan(DistanceRightTread * FMath::Sign(RightTreadSpeed) / RadiusRight);

			float RadiusCenter = RadiusRight + (TREAD_DISTANCE / 2);
			float MovementFromCenter = (DistanceRightTread * FMath::Sign(RightTreadSpeed) / RadiusRight) * RadiusCenter;
			FuturPosition += FVector(MovementFromCenter * FMath::Cos(FuturHeading), MovementFromCenter * FMath::Sin(FuturHeading), 0.0f);
		}
		//Pivot point is between the center of the robot and the right thread
		else
		{
			float LeftThreadToRotationPoint = (TREAD_DISTANCE * DistanceLeftTread * FMath::Sign(LeftTreadSpeed)) / (FMath::Abs(DistanceRightTread) + DistanceLeftTread * FMath::Sign(LeftTreadSpeed));
			float CenterToRotationPoint = LeftThreadToRotationPoint - (TREAD_DISTANCE / 2);

			float MovementFromCenter = (DistanceLeftTread * FMath::Sign(LeftTreadSpeed) / LeftThreadToRotationPoint) * CenterToRotationPoint;

			FuturHeading += FMath::Atan(MovementFromCenter / CenterToRotationPoint);
			FuturPosition += FVector(MovementFromCenter * FMath::Cos(FuturHeading), MovementFromCenter * FMath::Sin(FuturHeading), 0.0f);
		}
	}

	RobotSpeed = FVector::Distance(this->GetActorLocation(), FuturPosition) / DeltaTime;

	bCollisionThisTurn = false;
	//Robot - Robot Collision detection.
	if (UWorld* World = GetWorld())
	{
		TArray<FHitResult> OutHit;
		if (World->SweepMultiByProfile(OutHit, RobotDirection->GetComponentLocation(), FuturPosition, FQuat::Identity, RobotCollisionProfile, RobotCollisionCapsule->GetCollisionShape()))
		{
			for (FHitResult CurrentHit : OutHit)
			{
				if (UCapsuleComponent* CollisionChecker = Cast<UCapsuleComponent>(CurrentHit.GetComponent()))
				{
					if (ARobot* CurrentHitActor = Cast<ARobot>(CurrentHit.GetActor()))
					{
						if (CurrentHitActor->GetName().Compare(this->GetName()) != 0)
						{
							//Check for double collision.
							if (PreviousFrameCollisionRobot != nullptr)
							{
								if (CurrentHitActor->GetName().Compare(PreviousFrameCollisionRobot->GetName()) == 0)
								{
									break;
								}
								else
								{
									PreviousFrameCollisionRobot = nullptr;
								}
							}
							bCollisionThisTurn = true;
							//TODO Check recoil speed. Currently acceptable but might need to do some real math.
							FVector RecoilDirection = this->GetActorLocation() - CurrentHitActor->GetActorLocation();
							RecoilDirection.Normalize();

							PreviousFrameCollisionRobot = CurrentHitActor;

							float NewRecoilHeading = RecoilDirection.HeadingAngle();

							RecoilHeading = NewRecoilHeading;

							//This is not working properly but with Vector calculation it should be fine.
							/*if (RecoilVelocity == 0.0f)
							{
								RecoilHeading = NewRecoilHeading;
							}
							else
							{
								RecoilHeading = (RecoilHeading + NewRecoilHeading) / 2;
							}*/
							RecoilVelocity = BASE_RECOIL_VELOCITY;

							//TODO Damage should dvantage the Robot going faster.
							//Current idea: this->RobotSpeed / 2 + CurrentHitActor->GetRobotSpeed()
							GetHit(DAMAGE_ROBOT, this->RobotSpeed + CurrentHitActor->GetRobotSpeed());

							break;
						}
					}

				}
			}
		}
	}

	if (!bCollisionThisTurn)
	{
		PreviousFrameCollisionRobot = nullptr;
	}

	RobotDirection->SetWorldRotation(FRotator(0.0f, FMath::RadiansToDegrees(FuturHeading), 0.0f));
	SetActorLocation(FuturPosition);
}

/***********************************************************************************************

Mehtod:			UpdateSensor

Description:	This method check each Sensor for collision, update their data and their position
				in the RANGE Sensor case.

Parameters:		Nothing.

Returns:		Nothing.

Note:			Nothing.
***********************************************************************************************/
void ARobot::UpdateSensor()
{
	for (int32 i = 0; i < MAX_SENSORS; i++)
	{
		if (SensorArray[i]->GetTypeOfSensor() != SENSOR_NONE)
		{
			if (SensorArray[i]->IsEnoughEnergy() && SensorArray[i]->IsSensorOn())
			{
				//if the sensor was off or didn't have enough energy last turn and now everything is ON, render the mesh.
				if (SensorMeshArray[i]->bHiddenInGame)
				{
					SensorMeshArray[i]->SetHiddenInGame(false);
				}

				if (SensorArray[i]->GetTypeOfSensor() == SENSOR_RANGE)
				{
					//Update the line position of the sensor as if there is no collision.
					int32 SensorAngle = SensorArray[i]->GetSensorAngle();

					FVector PosStart = RobotDirection->GetComponentLocation();
					FVector PosEnd = PosStart + RobotDirection->GetComponentRotation().Vector() * SensorArray[i]->GetSensorRange();

					if (UWorld* World = GetWorld())
					{
						TArray<FHitResult> OutHit;

						FCollisionShape CollisionShape;
						CollisionShape.SetCapsule(5.0f, 200.0f);

						//Check if there is a collision and update the EndPos with the point of contact of the collision.
						//Since it can hit multiple AActor (including itself), I made the decision to stop after the first hit that is not itself.
						//Weirdly enough, I need to use the Projectile:Fly collision profile even though the RangeSensor is identical. This means RangeSensor can detect missile and laser. (For now)
						//TODO Change the collision to only hit the Robot.
						if (World->SweepMultiByProfile(OutHit, PosStart, PosEnd, FQuat(FRotator(0.0f, SensorArray[i]->GetSensorAngle(), 0.0f)), TEXT("Projectile:Fly"), CollisionShape))
						{
							for (int32 CurrentCollision = 0; CurrentCollision < OutHit.Num(); CurrentCollision++)
							{
								AActor* OtherActor = OutHit[CurrentCollision].GetActor();
								if (ARobot* temp = Cast<ARobot>(OtherActor))
								{
									if (OtherActor->GetName().Compare(this->GetName()) != 0)
									{
										PosEnd = OutHit[CurrentCollision].Location;
										break;
									}

								}
							}

						}
					}

					GetWorld()->LineBatcher->DrawLine(PosStart, PosEnd, RobotColor, 1, 2.0f);
					SensorMeshArray[i]->SetWorldLocation(PosEnd);
				}
			}
			else
			{
				SensorMeshArray[i]->SetHiddenInGame(true);
			}
			
		}
	}
}

/***********************************************************************************************

Mehtod:			UpdateEnergy

Description:	This method allocate the Energy Point to each of the System and then Update the
				Shield opacity based on the Energy Point stored in the Shield SYSTEM.

Parameters:		float DeltaTime		- The number of seconds between THIS frame and the LAST frame.

Returns:		Nothing.

Note:			Nothing.
***********************************************************************************************/
void ARobot::UpdateEnergy(float DeltaTime)
{
	EnergySystem->UpdateEnergySystem(DeltaTime, this);
	UpdateShield();
}


/***********************************************************************************************

Mehtod:			UpdateShield

Description:	This method update the opacity of the Shield SYSTEM based on the Energy Points
				stored.

Parameters:		Nothing.

Returns:		Nothing.

Note:			Nothing.

//TODO Test shield transparency value. 0.8f seems a little bit too much.
***********************************************************************************************/
void ARobot::UpdateShield()
{
	if ((EnergySystem->GetSystemEnergy(SYSTEM_SHIELDS) / SHIELDS_LEAK_THRESHOLD) < 1.0f)
	{
		if (RobotShield)
		{
			if (ShieldMaterial)
			{
				ShieldMaterial->SetScalarParameterValue("ShieldTransparency", EnergySystem->GetSystemEnergy(SYSTEM_SHIELDS) / SHIELDS_LEAK_THRESHOLD * 0.8f);
			}
		}
	}
}

/***********************************************************************************************

Mehtod:			UpdateInformation

Description:	This method is called every frame to update the Robot information. The UI can then
				access this information and display it on screen.

Parameters:		Nothing.

Returns:		Nothing.

Note:			- Not tested yet but, if the Robot is Destroyed while the TurboBoost is ON, it
might crash the Engine since it won't be able to call the Method on a
destroyed Object.
***********************************************************************************************/
void ARobot::UpdateInformation()
{
	CurrentGeneratorCapacity = EnergySystem->GetGeneratorStructure();
	CurrentShieldCharge = EnergySystem->GetSystemEnergy(SYSTEM_SHIELDS);
	CurrentLaserCharge = EnergySystem->GetSystemEnergy(SYSTEM_LASERS);
	CurrentMissileCharge = EnergySystem->GetSystemEnergy(SYSTEM_MISSILES);
}

/***********************************************************************************************

Mehtod:			KillThisRobot

Description:	This method is called when the Robot generator is <= 0. It spawn the explosion,
				set bIsAlive to flase and destroy the Object.

Parameters:		Nothing.

Returns:		Nothing.

Note:			Nothing.
***********************************************************************************************/
void ARobot::KillThisRobot()
{
	//TODO Remove the Robot, play explosion.
	bIsAlive = false;
	this->Destroy();
}

/***********************************************************************************************

Mehtod:			TurnBoosOff

Description:	This method is called after the the TurboBoostTimer is expired and turn the
				Turbo Boost OFF.

Parameters:		Nothing.

Returns:		Nothing.

Note:			- Not tested yet but, if the Robot is Destroyed while the TurboBoost is ON, it
					might crash the Engine since it won't be able to call the Method on a
					destroyed Object.
***********************************************************************************************/
void ARobot::TurnBoosOff()
{
	TurboOn = 0;
}

/***********************************************************************************************

Mehtod:			RadarOverlap

Description:	This method is called by the Engine each time something overlap the RADAR
				Sensors. It find the right Sensor, check if the overlapping Object is a Robot, 
				check if it's itself then adjust the data of the Sensor.

Parameters:		UPrimitiveComponent * OverlappedComponent		- A pointer to the overlapped component
				AActor * OtherActor								- A pointer to the AActor overlapping
				UPrimitiveComponent * OtherComp					- A pointer to the Component overlapping
				int32 OtherBodyIndex							- Don't know
				bool bFromSweep									- Don't know
				const FHitResult & SweepResult					- Don't know

Returns:		Nothing.

Note:			- The signature have to be this else it can't be called by the Engine. It
					sometimes change with Engine update.
***********************************************************************************************/
void ARobot::RadarOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//Check if the AActor overlaping is a Robot.
	if (ARobot* temp = Cast<ARobot>(OtherActor)) {
		//Iterate through all Sensor to find the overlapping Sensor
		for (int32 i = 0; i < MAX_SENSORS; i++)
		{
			if (SensorArray[i]->GetTypeOfSensor() == SENSOR_RADAR)
			{
				//Find the right Sensor and check if it's powered and turned on.
				if (Cast<UStaticMeshComponent>(OverlappedComponent) == SensorMeshArray[i] && !SensorMeshArray[i]->bHiddenInGame)
				{
					//Check if the Sensor is already set to 1.
					if (SensorArray[i]->GetSensorData() != 1)
					{
						//MAGIC NUMBER ALERT! Right now Radar Sensor can only detect Robot but in a futur iteration it might want to detect missiles or other new weapon.
						SensorArray[i]->SetSensorData(1);
					}
				}
			}
		}
	}
}

/***********************************************************************************************

Mehtod:			RadarOverlapEnd

Description:	This method is called by the Engine everytime there is an Object exiting a RADAR
				Sensor. This method make sure there's nothing overlapping it before changing the
				Sensor data.

Parameters:		UPrimitiveComponent * OverlappedComponent		- A pointer to the overlapped component
				AActor * OtherActor								- A pointer to the AActor overlapping
				UPrimitiveComponent * OtherComp					- A pointer to the Component overlapping
				int32 OtherBodyIndex							- Don't know

Returns:		Nothing.

Note:			- The signature have to be this else it can't be called by the Engine. It
					sometimes change with Engine update.
***********************************************************************************************/
void ARobot::RadarOverlapEnd(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	//Iterate through all Sensor to find the overlapping Sensor
	for (int32 i = 0; i < MAX_SENSORS; i++)
	{
		if (SensorArray[i]->GetTypeOfSensor() == SENSOR_RADAR)
		{
			//When it find the right Sensor, check if it's the right Sensor.
			if (Cast<UStaticMeshComponent>(OverlappedComponent) == SensorMeshArray[i])
			{
				TArray<AActor*> OverlappingActors;
				OverlappedComponent->GetOverlappingActors(OverlappingActors);
				
				//Get all the Actor overlapping the RadarSensor
				for (int32 CurrentOverlappingActor = 0; CurrentOverlappingActor < OverlappingActors.Num(); CurrentOverlappingActor++)
				{
					//Excluding the owner of the RadarSensor
					if (OverlappingActors[CurrentOverlappingActor] != this)
					{
						TArray<UPrimitiveComponent*> OverlappingComponents;
						OverlappedComponent->GetOverlappingComponents(OverlappingComponents);

						//GEtting all the Component overlapping the RadarSensor
						for (int32 CurrentOverlappingComponent = 0; CurrentOverlappingComponent < OverlappingComponents.Num(); CurrentOverlappingComponent++)
						{
							UCapsuleComponent* temp = Cast<UCapsuleComponent>(OverlappingComponents[CurrentOverlappingComponent]);

							//If the Component overlapping is a UCapsuleComponent then a Rabot is in the RadarSensor since Robot are the only Actor using UCapsuleComponent
							if (temp)
							{
								return;
							}
						}
					}
				}
				//MAGIC NUMBER ALERT! Nothing overlaping.
				SensorArray[i]->SetSensorData(0);
			}
		}
	}
}

/***********************************************************************************************

Mehtod:			Tick

Description:	This method is called every frame by the Engine. Every Object that can Tick has
				this method and it's ALWAYS called each frame.

Parameters:		float DeltaTime		- The number of seconds between THIS frame and the LAST frame.

Returns:		Nothing.

Note:			Nothing.
***********************************************************************************************/
// Called every frame
void ARobot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateEnergy(DeltaTime);
	MoveRobot(DeltaTime);
	UpdateSensor();
	UpdateInformation();

}

/***********************************************************************************************

Mehtod:			SetRobotColor

Description:	This method set the Robot collor.

Parameters:		FLinearColor color		- An RGBA representation of a collor.

Returns:		Nothing.

Note:			Nothing.
***********************************************************************************************/
void ARobot::SetRobotColor(FLinearColor color)
{
	RobotColor = color;

	if (ShieldMaterialHelper)
	{
		ShieldMaterial->SetVectorParameterValue("ShieldColor", RobotColor);
	}

	for (int32 i = 0; i < MAX_SENSORS; i++)
	{
		if (SensorArray[i]->GetTypeOfSensor() == SENSOR_RANGE) {
			if (RangeSensorMaterialHelper)
			{
				RangeSensorMaterial->SetVectorParameterValue("SensorColor", RobotColor);
			}
		}
		else if (SensorArray[i]->GetTypeOfSensor() == SENSOR_RADAR)
		{
			if (RadarSensorMaterialHelper)
			{
				RadarSensorMaterial->SetVectorParameterValue("ShieldColor", RobotColor);
			}
		}
	}
}

/***********************************************************************************************

Mehtod:			GetHit

Description:	This method is called when something hit the Robot. It first adjust the BumpData
				and then deal damage to the Robot.

Parameters:		DAMAGETYPE DamageType		- The type of damage dealt to the Robot.
				float DamageValue			- The damage taken by the Robot.

Returns:		Nothing.

Note:			Nothing.
***********************************************************************************************/
void ARobot::GetHit(DAMAGETYPE DamageType, float DamageValue)
{
	switch (DamageType)
	{
	case DAMAGE_MISSILE:
		BumpInfo |= BUMP_MISSILE;
		break;
	case DAMAGE_LASER:
		BumpInfo |= BUMP_LASER;
		break;
	case DAMAGE_ROBOT:
		BumpInfo |= BUMP_ROBOT;
		break;
	case DAMAGE_WALL:
		BumpInfo |= BUMP_WALL;
		break;
	default:
		break;
	}

	if (!EnergySystem->RemoveEnergy(DamageValue))
	{
		KillThisRobot();
	}

	UE_LOG(LogTemp, Warning, TEXT("%s got hit for %f damage"), *GetName(), DamageValue)
}

/***********************************************************************************************

Mehtod:			GetSensor

Description:	This method return the reference to the Sensor at Port index.

Parameters:		int32 index		- The port of the desired Sensor.

Returns:		Nothing.

Note:			Nothing.
***********************************************************************************************/
USensorSystem* ARobot::GetSensor(int32 index)
{
	return SensorArray[index];
}

/***********************************************************************************************

Mehtod:			GetRobotSpeed

Description:	This method return the speed of the Robot. The speed is calculated in MoveRobot()
				by getting the distance traveled by the Robot during DeltaTime.

Parameters:		Nothing.

Returns:		float		- The speed of the robot.

Note:			Nothing.
***********************************************************************************************/
float ARobot::GetRobotSpeed()
{
	return RobotSpeed;
}
