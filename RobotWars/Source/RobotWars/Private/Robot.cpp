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


	if (ShieldMaterialHelper)
	{
		ShieldMaterial = RobotShield->CreateDynamicMaterialInstance(0, ShieldMaterialHelper);
		RobotShield->SetMaterial(0, ShieldMaterial);
		ShieldMaterial->SetVectorParameterValue("ShieldColor", RobotColor);
	}



	LeftThreadSpeed = -100;
	RightThreadSpeed = 100;

	UE_LOG(LogTemp, Warning, TEXT("LeftThreadSpeed = %i   RightThreadSpeed = %i"), LeftThreadSpeed, RightThreadSpeed)

		//FireMissile();

		//RobotDirection->SetWorldRotation(FRotator(0.0f, 45.0f, 0.0f));
}

void ARobot::SetRobotName(FString RobotNewName)
{
	RobotName = RobotNewName;
}

// A method for the Robot to set each thread speed so the AI can set his speed.
void ARobot::SetThreadSpeed(float LeftThread, float RightThread)
{
	if (LeftThread < MIN_THREAH_SPEED)
	{
		LeftThread = MIN_THREAH_SPEED;
	}
	else if (LeftThread > MAX_THREAD_SPEED)
	{
		LeftThread = MAX_THREAD_SPEED;
	}

	if (RightThread < MIN_THREAH_SPEED)
	{
		RightThread = MIN_THREAH_SPEED;
	}
	else if (RightThread > MAX_THREAD_SPEED)
	{
		RightThread = MAX_THREAD_SPEED;
	}
	

	LeftThreadSpeed = LeftThread;
	RightThreadSpeed = RightThread;
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


//TODO The Robot is turning super quickly when one of the thread i negative.
//Might want to slow it down.
void ARobot::MoveRobot(float DeltaTime)
{
	float FuturHeading = 0.0f;
	FVector FuturPosition= FVector::ZeroVector;

	//We take the absolute value of the Thread speed because the direction as been
	//accounted for in the math behind the movement.
	float DistanceLeftThread = (FMath::Abs(LeftThreadSpeed) / MAX_THREAD_SPEED) * MAX_SPEED * DeltaTime;
	float DistanceRightThread = (FMath::Abs(RightThreadSpeed) / MAX_THREAD_SPEED) * MAX_SPEED * DeltaTime;

	if (LeftThreadSpeed == RightThreadSpeed)
	{
		FuturHeading = FMath::DegreesToRadians(RobotDirection->GetComponentRotation().Yaw);
		FuturPosition = FVector(DistanceLeftThread * FMath::Cos(FuturHeading), DistanceRightThread * FMath::Sin(FuturHeading), 0.0f) + GetActorLocation();
	}
	//Turn with pivot on the left thread
	else if (LeftThreadSpeed == 0)
	{
		//I substract the angle so the Robot turns left because the RightThreadSpeed > LeftTreadSpeed.
		FuturHeading = FMath::DegreesToRadians(RobotDirection->GetComponentRotation().Yaw) - FMath::Atan(DistanceRightThread / TREAD_DISTANCE);
		
		float MovementFromCenter = DistanceRightThread / 2;

		FuturPosition = FVector(MovementFromCenter * FMath::Cos(FuturHeading), MovementFromCenter * FMath::Sin(FuturHeading), 0.0f) + GetActorLocation();

	}
	//Turn with pivot on the right thread
	else if (RightThreadSpeed == 0)
	{
		FuturHeading = FMath::DegreesToRadians(RobotDirection->GetComponentRotation().Yaw) + FMath::Atan(DistanceLeftThread / TREAD_DISTANCE);

		float MovementFromCenter = DistanceLeftThread / 2;

		FuturPosition = FVector(MovementFromCenter * FMath::Cos(FuturHeading), MovementFromCenter * FMath::Sin(FuturHeading), 0.0f) + GetActorLocation();
	}
	//The Robot is turning left
	else if (RightThreadSpeed > LeftThreadSpeed)
	{
		if (DistanceLeftThread == DistanceRightThread)
		{
			FuturHeading = FMath::DegreesToRadians(RobotDirection->GetComponentRotation().Yaw) - FMath::Atan(DistanceRightThread / (TREAD_DISTANCE / 2));
			FuturPosition = GetActorLocation();
		}
		//Pivot point is outside of the robot which mean wide turn
		else if (LeftThreadSpeed > 0)
		{
		
			float RadiusRight = 1 / ((1 / TREAD_DISTANCE) * ((DistanceRightThread / DistanceLeftThread) - 1)) + 15;

			FuturHeading = FMath::DegreesToRadians(RobotDirection->GetComponentRotation().Yaw) - FMath::Atan(DistanceRightThread / RadiusRight);

			float RadiusCenter = RadiusRight - (TREAD_DISTANCE / 2);
			float MovementFromCenter = (DistanceRightThread / RadiusRight) * RadiusCenter;
			FuturPosition = FVector(MovementFromCenter * FMath::Cos(FuturHeading), MovementFromCenter * FMath::Sin(FuturHeading), 0.0f) + GetActorLocation();
		}
		//Pivot point is between the center of the robot and the left thread
		else
		{
			float RightThreadToRotationPoint = (TREAD_DISTANCE * DistanceRightThread) / (DistanceRightThread + DistanceLeftThread);
			float CenterToRotationPoint = RightThreadToRotationPoint - (TREAD_DISTANCE / 2);

			float MovementFromCenter = (DistanceRightThread / RightThreadToRotationPoint) * CenterToRotationPoint;

			FuturHeading = FMath::DegreesToRadians(RobotDirection->GetComponentRotation().Yaw) - FMath::Atan(MovementFromCenter / CenterToRotationPoint);
			FuturPosition = FVector(MovementFromCenter * FMath::Cos(FuturHeading), MovementFromCenter * FMath::Sin(FuturHeading), 0.0f) + GetActorLocation();
		}
	}
	//The Robot is turning Right
	else if (LeftThreadSpeed > RightThreadSpeed)
	{
		if (DistanceLeftThread == DistanceRightThread)
		{
			FuturHeading = FMath::DegreesToRadians(RobotDirection->GetComponentRotation().Yaw) + FMath::Atan(DistanceRightThread / (TREAD_DISTANCE / 2));
			FuturPosition = GetActorLocation();
		}
		//Pivot point is outside of the robot which mean wide turn
		else if (RightThreadSpeed > 0)
		{
			float RadiusRight = 1 / ((1 / TREAD_DISTANCE) * ((DistanceRightThread / DistanceLeftThread) - 1));

			FuturHeading = FMath::DegreesToRadians(RobotDirection->GetComponentRotation().Yaw) - FMath::Atan(DistanceRightThread / RadiusRight);

			float RadiusCenter = RadiusRight + (TREAD_DISTANCE / 2);
			float MovementFromCenter = (DistanceRightThread / RadiusRight) * RadiusCenter;
			FuturPosition = FVector(MovementFromCenter * FMath::Cos(FuturHeading), MovementFromCenter * FMath::Sin(FuturHeading), 0.0f) + GetActorLocation();
		}
		//Pivot point is between the center of the robot and the right thread
		else
		{
			float LeftThreadToRotationPoint = (TREAD_DISTANCE * DistanceLeftThread) / (DistanceRightThread + DistanceLeftThread);
			float CenterToRotationPoint = LeftThreadToRotationPoint - (TREAD_DISTANCE / 2);

			float MovementFromCenter = (DistanceLeftThread / LeftThreadToRotationPoint) * CenterToRotationPoint;

			FuturHeading = FMath::DegreesToRadians(RobotDirection->GetComponentRotation().Yaw) + FMath::Atan(MovementFromCenter / CenterToRotationPoint);
			FuturPosition = FVector(MovementFromCenter * FMath::Cos(FuturHeading), MovementFromCenter * FMath::Sin(FuturHeading), 0.0f) + GetActorLocation();
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

