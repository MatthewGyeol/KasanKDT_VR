// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveComponent.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "VRPlayer.h"
#include "MotionControllerComponent.h"
#include "GameFramework/PlayerController.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "TeleportRingActor.h"
#include "NiagaraComponent.h"
#include "BallActor.h"
#include "Components/SphereComponent.h"


UMoveComponent::UMoveComponent()
{
	PrimaryComponentTick.bCanEverTick = true;


}


void UMoveComponent::BeginPlay()
{
	Super::BeginPlay();

	player = GetOwner<AVRPlayer>();

	// �ڷ���Ʈ Ÿ�� ������ ǥ���� �� ����Ʈ�� ���� �� ��Ȱ��ȭ�Ѵ�.
	ringInstance = GetWorld()->SpawnActor<ATeleportRingActor>(ringActor, player->GetActorLocation(), player->GetActorRotation());
	ringInstance->ringFX->SetVisibility(false);

	// ���� �񱳿� �� ���� �����ϱ�
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ballInstance = GetWorld()->SpawnActor<ABallActor>(ballActor, player->rightMotion->GetComponentLocation(), FRotator::ZeroRotator, params);
}


void UMoveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsShowLine && player != nullptr)
	{
		UMotionControllerComponent* rightMotionCon = player->rightMotion;
		if (rightMotionCon != nullptr)
		{
			FVector handLocation = rightMotionCon->GetComponentLocation();
			FVector direction = rightMotionCon->GetForwardVector() + rightMotionCon->GetUpVector() * -1;
			DrawTrajectory(handLocation, direction.GetSafeNormal(), power, throwTime, throwTerm);
		}
	}

	if (!bShootBall)
	{
		ballInstance->SetActorLocation(player->rightMotion->GetComponentLocation());
	}
}

void UMoveComponent::SetupPlayerInputComponent(UEnhancedInputComponent* PlayerInputComponent, TArray<UInputAction*> inputs)
{
	PlayerInputComponent->BindAction(inputs[0], ETriggerEvent::Started, this, &UMoveComponent::ShootBall);
	PlayerInputComponent->BindAction(inputs[0], ETriggerEvent::Triggered, this, &UMoveComponent::ShowTrajectory);
	PlayerInputComponent->BindAction(inputs[0], ETriggerEvent::Completed, this, &UMoveComponent::TeleportToTarget);
}

void UMoveComponent::ShootBall()
{
	if (ballInstance != nullptr)
	{
		UMotionControllerComponent* rightMotionCon = player->rightMotion;
		if (rightMotionCon != nullptr)
		{
			FVector handLocation = rightMotionCon->GetComponentLocation();
			FVector direction = rightMotionCon->GetForwardVector() + rightMotionCon->GetUpVector() * -1;
			ballInstance->sphereComp->SetEnableGravity(true);
			ballInstance->sphereComp->AddImpulse(direction.GetSafeNormal() * power);
			bShootBall = true;
		}
	}
}

void UMoveComponent::ShowTrajectory()
{
	bIsShowLine = true;
}

// �������� �׸��� �Լ�(�ʿ��� ��: �÷��̾��� �� ��ġ, ���� ���� ����, ��, ���� �ð�, ���� Ƚ��)
void UMoveComponent::DrawTrajectory(FVector startLoc, FVector dir, float throwPower, float time, int32 term)
{
	float interval = time / (float)term;
	throwPoints.Empty();
	throwPoints.Add(startLoc);

	for (int32 i = 0; i < term; i++)
	{
		// p = p0 + vt - 0.5 * g * m * m * t * t
		float t = interval * i;
		float mass = ballInstance->sphereComp->GetMass();
		float gravity = 0.5f * GetWorld()->GetDefaultGravityZ() * mass * mass * t * t;
		FVector curLocation = startLoc + dir * throwPower * t + FVector(0, 0, gravity);

		// �� ���������� �浹 ���θ� üũ
		FHitResult hitInfo;
		FVector startVec = throwPoints[throwPoints.Num() - 1];

		if (GetWorld()->LineTraceSingleByChannel(hitInfo, startVec, curLocation, ECC_Visibility))
		{
			throwPoints.Add(hitInfo.ImpactPoint);
			break;
		}

		throwPoints.Add(curLocation);
	}

	if (throwPoints.Num() > 1)
	{
		for (int32 i = 0; i < throwPoints.Num() - 1; i++)
		{
			// DebugLine�� �̿��ؼ� �׸���
			//DrawDebugLine(GetWorld(), throwPoints[i], throwPoints[i + 1], FColor::Red, false, 0, 0, 2);

			// NiagaraSystem�� �̿��ؼ� �׸���
			UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(player->lineFX, FName("PointArray"), throwPoints);

			ringInstance->ringFX->SetVisibility(true);
			ringInstance->SetActorLocation(throwPoints[throwPoints.Num() - 1]);
		}
	}
}

// ������ ������ �ڷ���Ʈ�ϴ� �Լ�
void UMoveComponent::TeleportToTarget()
{
	bIsShowLine = false;
	// ���� ����Ʈ�� ���� ���� ���� 0���� �ʱ�ȭ�Ѵ�.
	TArray<FVector> initVector;
	initVector.SetNum(2);

	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(player->lineFX, FName("PointArray"), initVector);


	float fadeTime = 1.0f;

	if (player != nullptr && throwPoints.Num() > 0)
	{
		// 0.5�ʵ��� ���̵� �� ȿ���� �ְ� �̵��Ѵ�.
		//player->GetController<APlayerController>()->ClientSetCameraFade(true, FColor::White, FVector2D(0, 1.0f), fadeTime);
		player->GetController<APlayerController>()->PlayerCameraManager->StartCameraFade(0, 1, fadeTime, FLinearColor::White);

		//FColor myColor1 = FColor(255, 255, 255, 255);
		//FLinearColor myColor2 = FColor(1, 1, 1, 1);

		FTimerHandle teleportTimer;
		GetWorld()->GetTimerManager().SetTimer(teleportTimer, FTimerDelegate::CreateLambda([&]() {
			player->SetActorLocation(throwPoints[throwPoints.Num() - 1]);
			ringInstance->ringFX->SetVisibility(false);
			}), fadeTime, false);

	}
}



