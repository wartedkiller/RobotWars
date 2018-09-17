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
#include "EnergySystem.h"
#include "SensorSystem.h"

/****************************************************
IF ANYTHING CHANGE IN THE CONSTRUCTOR OF THIS CLASS,
THE ENGINE MUST BE RESTARTED TO SEE THE CHANGE.
FAILING TO DO SO WILL RESULT IN SEEING WEIRD RESULT
OF WHAT YOU ARE TRYING TO DO.
****************************************************/
ARobot::ARobot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

	//Creating the UStaticMeshComponent for the Shield and assingning a 2D plane
	//as it's mesh.
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

	static ConstructorHelpers::FObjectFinder<UMaterial> RangeSensorMaterialGetter(TEXT("Material'/Game/Material/ShieldMaterial.ShieldMaterial'"));
	if (RangeSensorMaterialGetter.Succeeded())
	{

		RangeSensorMaterialHelper = RangeSensorMaterialGetter.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not load the Shield material."))
	}

	//Creating a UMissileSystem so this Robot can Fire Missiles. Currently trying to solve the problem. See in BeginPlay().
	//MissileSystem = CreateDefaultSubobject<UMissileSystem>(TEXT("Missilesystem"));

	//Creating the UEnergySystem for the Robot.
	EnergySystem = CreateDefaultSubobject<UEnergySystem>(TEXT("EnergySystem"));

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
		RangeSensorMaterial->SetVectorParameterValue("ShieldColor", RobotColor);
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
}

void ARobot::SetRobotName(FString RobotNewName)
{
	RobotName = RobotNewName;
}

// A method for the Robot to set each thread speed so the Player can set his speed.
void ARobot::SetTreadSpeed(float LeftThread, float RightThread)
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

void ARobot::FireMissile()
{
	MissileSystem->Fire(this, GetActorLocation(), RobotDirection->GetComponentRotation());

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

int32 ARobot::AddSensor(int port, SENSORTYPE type, int angle, int width, int range)
{
	if (port < MAX_SENSORS)
	{
		if (SensorArray[port]->GetTypeOfSensor() == SENSOR_NONE)
		{
			if (type == SENSOR_RADAR)
			{
				//TODO Scale the sensor based on the width and range
				FString MeshPath = TEXT("StaticMesh'/Game/Mesh/RadarSensor.RadarSensor'");
				SensorMeshArray[port]->SetStaticMesh(Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *MeshPath)));
				SensorMeshArray[port]->SetWorldRotation(FRotator(0.0f, 90.0f + angle, 0.0f));
				SensorMeshArray[port]->SetMaterial(0, RadarSensorMaterial);
				return SensorArray[port]->AddSensor(type, angle, width, range);
			}
			else
			{
				//Add RangeSensor Mesh in the port slot
				return SensorArray[port]->AddSensor(type, angle, width, range);
			}
		}
	}
	return 0;
}

//TODO The Robot is turning super quickly when one of the thread is negative.
//Might want to slow it down.
void ARobot::MoveRobot(float DeltaTime)
{
	//Initializing initial variable so they can be used even if they don't change.
	//The FuturHeading is initialized at the current rotation of the Robot.
	//The FuturPosition is initialized at the current position in the Arena.
	float FuturHeading = FMath::DegreesToRadians(RobotDirection->GetComponentRotation().Yaw);
	FVector FuturPosition = GetActorLocation();

	//This is the lenght of the movement vector.
	//The direction (positive or negative) has been accounted for in the math.
	float DistanceLeftTread = (FMath::Abs(LeftTreadSpeed) / MAX_TREAD_SPEED) * MAX_SPEED * DeltaTime;
	float DistanceRightTread = (FMath::Abs(RightTreadSpeed) / MAX_TREAD_SPEED) * MAX_SPEED * DeltaTime;
	/*float DistanceLeftTread = (LeftTreadSpeed / MAX_TREAD_SPEED) * MAX_SPEED * DeltaTime;
	float DistanceRightTread = (RightTreadSpeed / MAX_TREAD_SPEED) * MAX_SPEED * DeltaTime;*/

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
		if (SensorArray[i]->GetTypeOfSensor() != SENSOR_NONE && SensorArray[i]->IsEnoughEnergy() && SensorArray[i]->IsSensorOn())
		{
			if (SensorArray[i]->GetTypeOfSensor() == SENSOR_RANGE)
			{
				int32 SensorAngle = SensorArray[i]->GetSensorAngle();

				FVector PosStart = RobotDirection->GetComponentLocation();
				FVector PosEnd = RobotDirection->GetForwardVector();

				PosEnd.X = FMath::Cos(FMath::DegreesToRadians(RobotDirection->GetComponentRotation().Yaw + SensorAngle));
				PosEnd.Y = FMath::Sin(FMath::DegreesToRadians(RobotDirection->GetComponentRotation().Yaw + SensorAngle));

				PosEnd = (PosEnd * RANGE_MAX_RANGE) + PosStart;
				GetWorld()->LineBatcher->DrawLine(PosStart, PosEnd, RobotColor, 1, 2.0f);
			}
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("Something went wrong with the sensors"))
		}
	}
}

void ARobot::UpdateEnergy(float DeltaTime)
{
	EnergySystem->UpdateEnergySystem(DeltaTime, this);
}

// Called every frame
void ARobot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateEnergy(DeltaTime);
	MoveRobot(DeltaTime);

	GetWorld()->LineBatcher->Flush();
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

void ARobot::GetHit()
{
	if (RobotShield)
	{
		if (ShieldMaterial)
		{
			FLinearColor CurrentColor = FLinearColor(ShieldMaterial->K2_GetVectorParameterValue("ShieldColor"));
			ShieldMaterial->SetVectorParameterValue("ShieldColor", FLinearColor(CurrentColor.R, CurrentColor.G, CurrentColor.B, 0.5f));
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("%s got hit"), *GetName())
}

USensorSystem* ARobot::GetSensor(int index)
{
	return SensorArray[index];
}

