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

// Sets default values
ARobot::ARobot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RobotBase"));
	}

	RobotDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("RobotDirection"));
	RobotDirection->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
	
	RobotSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("RobotSprite"));
	RobotSprite->AttachToComponent(RobotDirection, FAttachmentTransformRules::KeepWorldTransform);

	RobotCollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RobotCollisionCapsule"));
	RobotCollisionCapsule->AttachToComponent(RobotDirection, FAttachmentTransformRules::KeepWorldTransform);
	RobotCollisionCapsule->InitCapsuleSize(23.0f, 200.0f); 

	RobotShield = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RobotShield"));
	RobotShield->AttachToComponent(RobotDirection, FAttachmentTransformRules::KeepWorldTransform);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShieldVisualAsset(TEXT("/Engine/BasicShapes/Plane"));
	if (ShieldVisualAsset.Succeeded())
	{
		RobotShield->SetStaticMesh(ShieldVisualAsset.Object);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Missile Blueprint could not be found so NO MISSILE WILL SPAWN"))
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> ShieldMaterialGetter(TEXT("Material'/Game/Material/ShieldMaterial.ShieldMaterial'"));
	if (ShieldMaterialGetter.Succeeded())
	{

		ShieldMaterialHelper = ShieldMaterialGetter.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Missile Blueprint could not be found so NO MISSILE WILL SPAWN"))
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> MissileBP(TEXT("Blueprint'/Game/Blueprint/Missile_BP.Missile_BP'"));
	if (MissileBP.Succeeded())
	{
		MissileToSpawn = MissileBP.Object->GeneratedClass;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Missile Blueprint could not be found so NO MISSILE WILL SPAWN"))
	}

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

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("RobotCamera"));
	CameraComponent->bUsePawnControlRotation = false;
	CameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
	CameraComponent->OrthoWidth = 1024.0f;
	CameraComponent->AspectRatio = 4.0f / 3.0f;
	CameraComponent->AttachToComponent(SpringArm, FAttachmentTransformRules::KeepWorldTransform, SpringArm->SocketName);
	CameraComponent->SetWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));
	
	//MissileToSpawn = CreateDefaultSubobject<AMissile>(TEXT("RobotMissile"));
}

// Called when the game starts or when spawned
void ARobot::BeginPlay()
{
	Super::BeginPlay();

	SetRobotColor(FLinearColor::Blue);

	RobotSprite->SetRelativeLocation(FVector(-5, 0, 1));

	if (ShieldMaterialHelper)
	{
		ShieldMaterial = RobotShield->CreateDynamicMaterialInstance(0, ShieldMaterialHelper);
		RobotShield->SetMaterial(0, ShieldMaterial);
		ShieldMaterial->SetVectorParameterValue("ShieldColor", RobotColor);
		RobotShield->SetWorldScale3D(FVector(0.55f, 0.55f, 0.55f));
	}

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

bool ARobot::FireMissile()
{
	if (MissileToSpawn)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			
			FRotator CurrentRotation = RobotDirection->GetComponentRotation();

			FVector CurrentPosition = GetActorLocation();

			World->SpawnActor<AMissile>(MissileToSpawn, CurrentPosition, CurrentRotation, SpawnParams);
			UE_LOG(LogTemp, Warning, TEXT("Missile Fire"))
			return true;
		}
	}
	return false;
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
	/*float DistanceLeftTread = (FMath::Abs(LeftTreadSpeed) / MAX_TREAD_SPEED) * MAX_SPEED * DeltaTime;
	float DistanceRightTread = (FMath::Abs(RightTreadSpeed) / MAX_TREAD_SPEED) * MAX_SPEED * DeltaTime;*/
	float DistanceLeftTread = (LeftTreadSpeed / MAX_TREAD_SPEED) * MAX_SPEED * DeltaTime;
	float DistanceRightTread = (RightTreadSpeed / MAX_TREAD_SPEED) * MAX_SPEED * DeltaTime;

	//Case 1 : The Robot is moving without turning.
	//There is no math for this case since it's trivial.
	if (LeftTreadSpeed == RightTreadSpeed)
	{
		FuturPosition += FVector(DistanceLeftTread * FMath::Cos(FuturHeading), DistanceRightTread * FMath::Sin(FuturHeading), 0.0f);
	}
	//Case 2 : One of the tread is not moving therefore the pivot point is at the position of the immobile tread.
	///Case 2 RightTread
	else if (LeftTreadSpeed == 0)
	{
		//I substract the angle so the Robot turns left because the RightThreadSpeed > LeftTreadSpeed.
		FuturHeading -= FMath::Atan(DistanceRightTread / TREAD_DISTANCE);
		
		float MovementFromCenter = DistanceRightTread / 2;

		FuturPosition += FVector(MovementFromCenter * FMath::Cos(FuturHeading), MovementFromCenter * FMath::Sin(FuturHeading), 0.0f);

	}
	///Case 2 LeftTread
	else if (RightTreadSpeed == 0)
	{
		FuturHeading += FMath::Atan(DistanceLeftTread / TREAD_DISTANCE);

		float MovementFromCenter = DistanceLeftTread / 2;

		FuturPosition += FVector(MovementFromCenter * FMath::Cos(FuturHeading), MovementFromCenter * FMath::Sin(FuturHeading), 0.0f);
	}
	//The Robot is turning left
	else if (RightTreadSpeed > LeftTreadSpeed)
	{
		//Case 3 : The Robot is turning on itself. The pivot point is in the middile of the Robot.
		///Case 3 RightTread
		if (DistanceLeftTread == DistanceRightTread)
		{
			FuturHeading -= FMath::Atan(DistanceRightTread / (TREAD_DISTANCE / 2));
		}
		//Case 4 : Left tread speed is positive so the pivot point is outside of the Robot.
		///Case 4 RightTread
		else if (LeftTreadSpeed > 0)
		{
			float RadiusLeft = TREAD_DISTANCE / ((DistanceRightTread / DistanceLeftTread) - 1);
			float RadiusCenter = RadiusLeft + (TREAD_DISTANCE / 2);
			float MovementFromCenter = (DistanceLeftTread * RadiusCenter) / RadiusLeft;

			FuturHeading -= FMath::Atan(MovementFromCenter / RadiusCenter);
			FuturPosition += FVector(MovementFromCenter * FMath::Cos(FuturHeading), MovementFromCenter * FMath::Sin(FuturHeading), 0.0f);
		}
		//Case 5 : Left tread is negative so the pivot is inside the Robot.
		///Case 5 RightTread
		else
		{
			float RightThreadToRotationPoint = (TREAD_DISTANCE * DistanceRightTread) / (DistanceRightTread + FMath::Abs(DistanceLeftTread));
			float CenterToRotationPoint = RightThreadToRotationPoint - (TREAD_DISTANCE / 2);

			float MovementFromCenter = (DistanceRightTread / RightThreadToRotationPoint) * CenterToRotationPoint;

			FuturHeading -= FMath::Atan(MovementFromCenter / CenterToRotationPoint);
			FuturPosition += FVector(MovementFromCenter * FMath::Cos(FuturHeading), MovementFromCenter * FMath::Sin(FuturHeading), 0.0f);
		}
	}
	//The Robot is turning Right
	else if (LeftTreadSpeed > RightTreadSpeed)
	{
		if (DistanceLeftTread == DistanceRightTread)
		{
			FuturHeading += FMath::Atan(DistanceRightTread / (TREAD_DISTANCE / 2));
		}
		//Pivot point is outside of the robot which mean wide turn
		else if (RightTreadSpeed > 0)
		{
			float RadiusRight = 1 / ((1 / TREAD_DISTANCE) * ((DistanceRightTread / DistanceLeftTread) - 1));

			FuturHeading -= FMath::Atan(DistanceRightTread / RadiusRight);

			float RadiusCenter = RadiusRight + (TREAD_DISTANCE / 2);
			float MovementFromCenter = (DistanceRightTread / RadiusRight) * RadiusCenter;
			FuturPosition += FVector(MovementFromCenter * FMath::Cos(FuturHeading), MovementFromCenter * FMath::Sin(FuturHeading), 0.0f);
		}
		//Pivot point is between the center of the robot and the right thread
		else
		{
			float LeftThreadToRotationPoint = (TREAD_DISTANCE * DistanceLeftTread) / (FMath::Abs(DistanceRightTread) + DistanceLeftTread);
			float CenterToRotationPoint = LeftThreadToRotationPoint - (TREAD_DISTANCE / 2);

			float MovementFromCenter = (DistanceLeftTread / LeftThreadToRotationPoint) * CenterToRotationPoint;

			FuturHeading += FMath::Atan(MovementFromCenter / CenterToRotationPoint);
			FuturPosition += FVector(MovementFromCenter * FMath::Cos(FuturHeading), MovementFromCenter * FMath::Sin(FuturHeading), 0.0f);
		}
	}

	RobotDirection->SetWorldRotation(FRotator(0.0f, FMath::RadiansToDegrees(FuturHeading), 0.0f));
	SetActorLocation(FuturPosition);
}

void ARobot::UpdateSensor()
{
	FVector PosStart = RobotDirection->GetComponentLocation();
	FVector PosEnd1 = RobotDirection->GetForwardVector();

	PosEnd1.X = FMath::Cos(FMath::DegreesToRadians(RobotDirection->GetComponentRotation().Yaw + 45.0f));
	PosEnd1.Y = FMath::Sin(FMath::DegreesToRadians(RobotDirection->GetComponentRotation().Yaw + 45.0f));

	PosEnd1 = (PosEnd1 * 125.0f) + PosStart;
	GetWorld()->LineBatcher->DrawLine(PosStart, PosEnd1, FLinearColor::Blue, 1, 1.0f);
}

// Called every frame
void ARobot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

