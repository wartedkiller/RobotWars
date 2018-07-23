// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Robot.generated.h"

UCLASS()
class ROBOTWARS_API ARobot : public APawn
{
	GENERATED_BODY()

///********** METHOD **********
public:
	// Sets default values for this pawn's properties
	ARobot();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void SetRobotName(FString RobotNewName);

///********** VARIABLE **********

public:
	FString RobotName = TEXT("DEFAULT");
	FVector2D RobotPosition;
	float RobotHeading;

protected:
	int LeftThreadSpeed;
	int RightThreadSpeed;

	// Debug tool - Show which way the Robot is facing
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		class UArrowComponent* RobotDirection;

	//Sprite for the Robot.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		class UPaperSpriteComponent* RobotSprite;

private:

	//The in Development Camera - Should be remove and put in an observer in final version
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* CameraComponent;
	
	//The in Development Springarm - Should be remove and put in an observer in final version
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* SpringArm;
};
