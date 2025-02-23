// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickUpActor.generated.h"

UCLASS()
class KDT_VR_API APickUpActor : public AActor
{
	GENERATED_BODY()
	
public:	
	APickUpActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category="MySettings")
	class UBoxComponent* boxComp;

	UPROPERTY(VisibleAnywhere, Category = "MySettings")
	class UStaticMeshComponent* meshComp;

	UPROPERTY(EditAnywhere, Category = "MySettings")
	FVector offsetLocation;

	UPROPERTY(EditAnywhere, Category = "MySettings")
	FRotator offsetRotation;

	UPROPERTY(EditAnywhere, Category = "MySettings")
	float throwPower = 500;

	UPROPERTY(EditAnywhere, Category = "MySettings")
	float torquePower = 100;

	void OnGrabbed(class USkeletalMeshComponent* handMeshComp);
	void OnReleased(FVector deltaDir, float throwThreshold, FVector deltaRot);
};
