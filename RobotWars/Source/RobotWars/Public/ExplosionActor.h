// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosionActor.generated.h"

UCLASS()
class ROBOTWARS_API AExplosionActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplosionActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

/**************************Variable***********************************/

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
		class UPaperFlipbookComponent* ExplosionFlipbook;

};
