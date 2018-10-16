// Fill out your copyright notice in the Description page of Project Settings.

#include "Robot.h"
#include "PaperSpriteComponent.h"
#include "Components/ArrowComponent.h"
#include "Camera/CameraComponent.h"
#include "RobotWarsStatics.h"
#include "GameFramework/SpringArmComponent.h"
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

//TODO Add LaserSystem and test it.

/****************************************************
IF ANYTHING CHANGE IN THE CONSTRUCTOR OF THIS CLASS,
THE ENGINE MUST BE RESTARTED TO SEE THE CHANGE.
FAILING TO DO SO WILL RESULT IN SEEING WEIRD RESULT
OF WHAT YOU ARE TRYING TO DO.
****************************************************/
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
	RobotDirection->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
	
	//Creating the default Robot sprite.
	//NOTE: Paper2D is an abandon ware in Unreal 4. While it will always be
	//in the Engine, it won't receive future upgrade. A solution to this is
	//using a 3D plane like it's done for the shield.
	RobotMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RobotMesh"));
	RobotMesh->AttachToComponent(RobotDirection, FAttachmentTransformRules::KeepWorldTransform);
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
	RobotCollisionCapsule->AttachToComponent(RobotDirection, FAttachmentTransformRules::KeepWorldTransform);
	RobotCollisionCapsule->InitCapsuleSize(23.0f, 200.0f); 

	//Creating the UStaticMeshComponent for the Shield.
	RobotShield = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RobotShield"));
	RobotShield->AttachToComponent(RobotDirection, FAttachmentTransformRules::KeepWorldTransform);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShieldVisualAsset(TEXT("StaticMesh'/Game/Mesh/Shield.Shield'"));
	if (ShieldVisualAsset.Succeeded())
	{
		RobotShield->SetStaticMesh(ShieldVisualAsset.Object);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not load static mesh for the Shield"))
	}

	//Loading the Shield material to be used in the BeginPlay() method.
	static ConstructorHelpers::FObjectFinder<UMaterial> ShieldMaterialGetter(TEXT("Material'/Game/Material/ShieldMaterial.ShieldMaterial'"));
	if (ShieldMaterialGetter.Succeeded())
	{
		ShieldMaterialHelper = ShieldMaterialGetter.Object;
		RadarSensorMaterialHelper = ShieldMaterialGetter.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not load the Shield material."))
	}

	//Loading the RangeSensor material to be used in the BeginPlay() method.
	static ConstructorHelpers::FObjectFinder<UMaterial> RangeSensorMaterialGetter(TEXT("Material'/Game/Material/RangeSensorMaterial_MAT.RangeSensorMaterial_MAT'"));
	if (RangeSensorMaterialGetter.Succeeded())
	{

		RangeSensorMaterialHelper = RangeSensorMaterialGetter.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not load the RangeSensor material."))
	}

	//Initializing Sensors
	SensorMeshArray[0] = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SensorMesh0"));
	SensorMeshArray[0]->AttachToComponent(RobotDirection, FAttachmentTransformRules::KeepWorldTransform);
	SensorMeshArray[1] = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SensorMesh1"));
	SensorMeshArray[1]->AttachToComponent(RobotDirection, FAttachmentTransformRules::KeepWorldTransform);
	SensorMeshArray[2] = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SensorMesh2"));
	SensorMeshArray[2]->AttachToComponent(RobotDirection, FAttachmentTransformRules::KeepWorldTransform);
	SensorMeshArray[3] = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SensorMesh3"));
	SensorMeshArray[3]->AttachToComponent(RobotDirection, FAttachmentTransformRules::KeepWorldTransform);


	//This will be remove and placed in the spectator.
	///If something is changed to the SpringArm or the CameraComponent, you must restart UE4
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetArmLength = 500.0f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = false;
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->CameraLagSpeed = 2.0f;
	SpringArm->bDoCollisionTest = false;
	SpringArm->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
	SpringArm->SetWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));

	//This will be remove and placed in the spectator.
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("RobotCamera"));
	CameraComponent->bUsePawnControlRotation = false;
	CameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
	CameraComponent->OrthoWidth = 1024.0f;
	CameraComponent->AspectRatio = 4.0f / 3.0f;
	CameraComponent->AttachToComponent(SpringArm, FAttachmentTransformRules::KeepWorldTransform, SpringArm->SocketName);
	CameraComponent->SetWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));
}

// Called when the game starts or when spawned
void ARobot::BeginPlay()
{
	Super::BeginPlay();
	
	//Temporary call of SetRobotColor(). The final color will be set by the GameMode when the game begin.
	SetRobotColor(FLinearColor::Blue);

	//Creating a dynamic material so we can change the color (the opacity in our case) in real time.
	//Also change the color of the shield and scales it to it's final size.
	if (ShieldMaterialHelper)
	{
		ShieldMaterial = RobotShield->CreateDynamicMaterialInstance(0, ShieldMaterialHelper);
		RobotShield->SetMaterial(0, ShieldMaterial);

		ShieldMaterial->SetVectorParameterValue("ShieldColor", RobotColor);
		RobotShield->SetWorldScale3D(FVector(1.0f));
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
}

void ARobot::SetRobotName(FString RobotNewName)
{
	RobotName = RobotNewName;
}

void ARobot::SetStatusMessage(char* Message)
{
	StatusMessage =  Message;
}

float ARobot::GetRandomNumber(int32 UpperBound)
{
	return FMath::RandRange(0, UpperBound);
}

// A method for the Robot to set each thread speed so the Player can set his speed.
void ARobot::SetMotorSpeeds(int32 LeftThread, int32 RightThread)
{
	if (LeftThread < MIN_THREAH_SPEED)
	{
		LeftThread = MIN_THREAH_SPEED;
	}
	else if (LeftThread > MAX_TREAD_SPEED)
	{
		LeftThread = MAX_TREAD_SPEED;
	}

	if (RightThread < MIN_THREAH_SPEED)
	{
		RightThread = MIN_THREAH_SPEED;
	}
	else if (RightThread > MAX_TREAD_SPEED)
	{
		RightThread = MAX_TREAD_SPEED;
	}
	

	LeftTreadSpeed = LeftThread;
	RightTreadSpeed = RightThread;
}

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

int32 ARobot::IsTurboOn()
{
	return TurboOn;
}

void ARobot::FireWeapon(WEAPONTYPE type, int32 heading)
{
	heading = heading % 360;

	switch (type)
	{
	case WEAPON_MISSILE:
		if (heading >= -MISSILE_MAX_ANGLE && heading <= MISSILE_MAX_ANGLE)
		{
			FRotator MissileHeading = RobotDirection->GetComponentRotation();
			MissileHeading.Yaw += heading;
			//TODO Remove Energy when missile is fired;
			MissileSystem->Fire(this, GetActorLocation(), MissileHeading);
		}
		break;
	case WEAPON_LASER:
		if (true)
		{
			FRotator LaserHeading = RobotDirection->GetComponentRotation();
			LaserHeading.Yaw += heading;

			//TODO Make sure the conversion from float to int32 != 0 when getting the LsserDamage
			int32 LaserDamage = EnergySystem->GetLaserDamage();

			LaserSystem->Fire(this, GetActorLocation(), LaserHeading, LaserDamage);
		}
		break;
	default:
		break;
	}
}

int32 ARobot::GetGeneratorStructur()
{
	return EnergySystem->GetGeneratorStructur();
}

int32 ARobot::GetGeneratorOutput()
{
	return EnergySystem->GetGeneratorOutput();
}

int32 ARobot::SetSystemChargePriorites(SYSTEM priorities[NUM_ENERGY_SYSTEMS])
{
	return EnergySystem->SetSystemChargePriorites(priorities);
}


float ARobot::GetSystemEnergy(SYSTEM type)
{
	return EnergySystem->GetSystemEnergy(type);
}

void ARobot::SetSystemChargeRate(SYSTEM type, int32 rate)
{
	EnergySystem->SetSystemChargeRate(type, rate);
}

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
				SensorMeshArray[port]->SetWorldRotation(FRotator(0.0f, angle, 0.0f));
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

int32 ARobot::GetBumpInfo()
{
	int32 temp = BumpInfo;
	BumpInfo = 0;
	return temp;
}

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

//TODO The Robot is turning super quickly when one of the thread is negative. Might want to slow it down.
//TODO Robot-Robot collision.
void ARobot::MoveRobot(float DeltaTime)
{
	//Initializing initial variable so they can be used even if they don't change.
	//The FuturHeading is initialized at the current rotation of the Robot.
	//The FuturPosition is initialized at the current position in the Arena.
	float FuturHeading = FMath::DegreesToRadians(RobotDirection->GetComponentRotation().Yaw);
	FVector FuturPosition = GetActorLocation();

	float DistanceLeftTread = 0.0f;
	float DistanceRightTread = 0.0f;

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

	RobotDirection->SetWorldRotation(FRotator(0.0f, FMath::RadiansToDegrees(FuturHeading), 0.0f));
	SetActorLocation(FuturPosition);
}


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

void ARobot::UpdateEnergy(float DeltaTime)
{
	EnergySystem->UpdateEnergySystem(DeltaTime, this);
	UpdateShield();
}


//TODO Test shield transparency value. 0.8f seems a little bit too much.
void ARobot::UpdateShield()
{
	if (RobotShield)
	{
		if (ShieldMaterial)
		{
			ShieldMaterial->SetScalarParameterValue("ShieldTransparency", EnergySystem->GetSystemEnergy(SYSTEM_SHIELDS) / MAX_SHIELD_ENERGY * 0.8f);
		}
	}
}

void ARobot::TurnBoosOff()
{
	TurboOn = 0;
}

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

// Called every frame
void ARobot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateEnergy(DeltaTime);
	MoveRobot(DeltaTime);
	UpdateSensor();

}

// Called to bind functionality to input
void ARobot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ARobot::SetRobotColor(FLinearColor color)
{
	RobotColor = color;
}


void ARobot::GetHit(DAMAGETYPE DamageType, int32 DamageValue)
{
	switch (DamageType)
	{
	case MISSILE:
		BumpInfo |= BUMP_MISSILE;
		break;
	case LASER:
		BumpInfo |= BUMP_LASER;
		break;
	case ROBOT:
		BumpInfo |= BUMP_ROBOT;
		break;
	case WALL:
		BumpInfo |= BUMP_WALL;
		break;
	default:
		break;
	}

	if (!EnergySystem->RemoveEnergy(DamageValue))
	{
		//TODO Destroy the robot.
		return;
	}
	UpdateShield();

	UE_LOG(LogTemp, Warning, TEXT("%s got hit"), *GetName())
}

USensorSystem* ARobot::GetSensor(int index)
{
	return SensorArray[index];
}
