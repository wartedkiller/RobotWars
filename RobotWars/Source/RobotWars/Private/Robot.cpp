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
	RobotCollisionCapsule->InitCapsuleSize(20.0f, 200.0f);


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

	LeftThreadSpeed = 100;
	RightThreadSpeed = 100;

	UE_LOG(LogTemp, Warning, TEXT("LeftThreadSpeed = %i   RightThreadSpeed = %i"), LeftThreadSpeed, RightThreadSpeed)
		
	FireMissile();
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


//TODO Something with turning logic is wrong which make the radius at which the robot turn weird.
//Like if the radius is always the same no mater the TreadSpeed.
//But they are turning the right way.
//Revisit the turning angle calculation.
void ARobot::MoveRobot(float DeltaTime)
{

	FVector DesiredMovementDirection;
	FVector CurrentPosition;
	FVector DesiredPosition;
	float DistanceTreadMiddle = 0.0f;
	float RotationAngle = 0.0f;
	float StartAngle = 0.0f;
	float InnerRadians = 0.0f;
	float CurrentHeading = 0.0f;
	float x = 0.0f;
	float y = 0.0f;
	float u = 0.0f;
	float v = 0.0f;
	

	float lTreadSpeed = LeftThreadSpeed;
	float rTreadSpeed = RightThreadSpeed;

	float lTreadDist = lTreadSpeed;
	float rTreadDist = rTreadSpeed;


	//Setting the direction and the position of the Robot for the next frame
	if (lTreadSpeed == rTreadSpeed) 
	{
		//Since there is no turning involved here, we simply get the current
		//heading of the Robot and calculate it's futur position.
		CurrentHeading = FMath::DegreesToRadians(RobotDirection->GetComponentRotation().Yaw);
		DesiredMovementDirection = FVector(lTreadDist * FMath::Cos(CurrentHeading), rTreadDist * FMath::Sin(CurrentHeading), 0.0f);
	}
	else
	{
		if (rTreadSpeed == 0)
		{
			DistanceTreadMiddle = TREAD_DISTANCE / 2;
			RotationAngle = lTreadDist * 360.0f / (2 * PI * TREAD_DISTANCE);
			StartAngle = RobotDirection->GetComponentRotation().Yaw + 270.0f;
		}
		else if (lTreadSpeed == 0)
		{
			DistanceTreadMiddle = TREAD_DISTANCE / 2;
			RotationAngle = -rTreadDist * 360.0f / (2 * PI * TREAD_DISTANCE);
			StartAngle = RobotDirection->GetComponentRotation().Yaw + 90.0f;
		}
		else
		{
			if (FMath::Abs(lTreadSpeed) > FMath::Abs(rTreadSpeed))
			{
				InnerRadians = rTreadDist * TREAD_DISTANCE / (lTreadDist - rTreadDist);
				DistanceTreadMiddle = InnerRadians + TREAD_DISTANCE / 2;
				RotationAngle = -rTreadDist * 360.0f / (2 * PI * InnerRadians);
				StartAngle = RobotDirection->GetComponentRotation().Yaw + 270.0f;
			}
			else
			{
				InnerRadians = lTreadDist * TREAD_DISTANCE / (rTreadDist - lTreadDist);
				DistanceTreadMiddle = InnerRadians + TREAD_DISTANCE / 2;
				RotationAngle = -lTreadDist * 360.0f / (2 * PI * InnerRadians);
				StartAngle = RobotDirection->GetComponentRotation().Yaw + 90.0f;

			}
		}

		StartAngle = FMath::DegreesToRadians(StartAngle);
		x = DistanceTreadMiddle * FMath::Cos(StartAngle);
		y = DistanceTreadMiddle * FMath::Sin(StartAngle);

		RotationAngle = FMath::DegreesToRadians(RotationAngle);
		u = x * FMath::Cos(RotationAngle) - y * FMath::Sin(RotationAngle);
		v = y * FMath::Cos(RotationAngle) + x * FMath::Sin(RotationAngle);
		DesiredMovementDirection = FVector(u - x, v - y, 0.0f);
	}

	
	//This is taken straight from Unreals "Tank VS Zombies" Tutorials.
	//Might need to check if things are really necessary in our case but for now it "works".
	//TODO Modify comments to match this game
	if (!DesiredMovementDirection.IsNearlyZero())
	{
		// Rotate the tank. This process deals with TankDirection directly, not the actor/RootComponent, because we don't want to affect the camera.
		FRotator MovementAngle = DesiredMovementDirection.Rotation();
		FRotator CurrentAngle = RobotDirection->GetComponentRotation();
		float DeltaYaw = URobotWarsStatics::FindDeltaAngleDegrees(CurrentAngle.Yaw, MovementAngle.Yaw);
		if (DeltaYaw != 0.0f)
		{
			float AdjustedDeltaYaw = DeltaYaw * DeltaTime;

			// Adjust toward our desired angle, and stop if we've reached it.
			float MaxYawThisFrame = 180.0f * DeltaTime;
			UE_LOG(LogTemp, Warning, TEXT("MaxYawThisTurn = %f    AdjustedYaw = %f    MovementAngle = %f"), MaxYawThisFrame, AdjustedDeltaYaw, MovementAngle.Yaw)
			if (MaxYawThisFrame > FMath::Abs(AdjustedDeltaYaw))
			{
				RobotDirection->SetWorldRotation(MovementAngle);
			}
			else
			{
				// Adjust as far as we can this frame, because we know we can't reach the goal yet.
				RobotDirection->AddLocalRotation(FRotator(0.0f, FMath::Sign(AdjustedDeltaYaw) * MaxYawThisFrame, 0.0f));
			}

			//UE_LOG(LogTemp, Warning, TEXT("MovementAngle = %f    RobotLocation = %f"), MovementAngle.Yaw, RobotDirection->GetComponentRotation().Yaw)
		}

		// Move the tank
		{
			FVector MovementDirection = RobotDirection->GetForwardVector();
			FVector Pos = GetActorLocation();
			Pos.X += MovementDirection.X * 100.0f * DeltaTime;
			Pos.Y += MovementDirection.Y * 100.0f * DeltaTime;
			SetActorLocation(Pos);
		}
	}
}

// Called every frame
void ARobot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//MoveRobot(DeltaTime);

	FVector PosStart = GetActorLocation();

	drawLine += DeltaTime * 2;

	FVector PosEnd = PosStart + FVector(45.0f, 45.0f - drawLine, 0.0f);

	//GetWorld()->LineBatcher->DrawLine(PosStart, PosEnd, FLinearColor::Blue, 1, 1.0f);
}

// Called to bind functionality to input
void ARobot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

