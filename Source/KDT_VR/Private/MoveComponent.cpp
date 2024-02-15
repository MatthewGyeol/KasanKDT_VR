// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveComponent.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "VRPlayer.h"
#include "MotionControllerComponent.h"


UMoveComponent::UMoveComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UMoveComponent::BeginPlay()
{
	Super::BeginPlay();


}


void UMoveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsShowLine)
	{
		UMotionControllerComponent* rightMotionCon = GetOwner<AVRPlayer>()->rightMotion;
		if (rightMotionCon != nullptr)
		{
			FVector handLocation = rightMotionCon->GetComponentLocation();
			FVector direction = rightMotionCon->GetForwardVector() + rightMotionCon->GetUpVector() * -1;
			DrawTrajectory(handLocation, direction.GetSafeNormal(), power, throwTime, throwTerm);
		}
	}
}

void UMoveComponent::SetupPlayerInputComponent(UEnhancedInputComponent* PlayerInputComponent, TArray<UInputAction*> inputs)
{
	PlayerInputComponent->BindAction(inputs[0], ETriggerEvent::Triggered, this, &UMoveComponent::ShowTrajectory);
	PlayerInputComponent->BindAction(inputs[0], ETriggerEvent::Completed, this, &UMoveComponent::TeleportToTarget);
}

void UMoveComponent::ShowTrajectory()
{
	bIsShowLine = true;
}

// �������� �׸��� �Լ�(�ʿ��� ��: �÷��̾��� �� ��ġ, ���� ���� ����, ��, ���� �ð�, ���� Ƚ��)
void UMoveComponent::DrawTrajectory(FVector startLoc, FVector dir, float throwPower, float time, int32 term)
{
	float interval = time / (float)term;
	TArray<FVector> throwPoints;

	for (int32 i = 0; i < term; i++)
	{
		// p = p0 + vt - 0.5 * g * t * t
		float t = interval * i;
		float gravity = 0.5f * GetWorld()->GetDefaultGravityZ() * t * t;
		FVector curLocation = startLoc + dir * throwPower * t + FVector(0, 0, gravity);
		throwPoints.Add(curLocation);
	}

	for (int32 i = 0; i < throwPoints.Num() - 1; i++)
	{
		DrawDebugLine(GetWorld(), throwPoints[i], throwPoints[i + 1], FColor::Red, false, 0, 0, 2);
	}
}

// ������ ������ �ڷ���Ʈ�ϴ� �Լ�
void UMoveComponent::TeleportToTarget()
{
	bIsShowLine = false;

}



