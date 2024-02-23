// Fill out your copyright notice in the Description page of Project Settings.


#include "GrabComponent.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "VRPlayer.h"
#include "PickUpActor.h"
#include "Components/SkeletalMeshComponent.h"


UGrabComponent::UGrabComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UGrabComponent::BeginPlay()
{
	Super::BeginPlay();

	player = GetOwner<AVRPlayer>();

}


void UGrabComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ������ ��Ʈ�ѷ��� ��ġ ���� �����Ѵ�.
	previousLocation_rightCon = currentLocation_rightCon;
	currentLocation_rightCon = player->rightHand->GetComponentLocation();

	//FVector deltaDirection = currentLocation_rightCon - previousLocation_rightCon;
	//UE_LOG(LogTemp, Warning, TEXT("delta length: %f"), deltaDirection.Length());

	// ������ ��Ʈ�ѷ��� ȸ�� ���� �����Ѵ�.
	previousRotation_rightCon = currentRotation_rightCon;
	currentRotation_rightCon = player->rightHand->GetComponentQuat();
}

void UGrabComponent::SetupPlayerInputComponent(UEnhancedInputComponent* PlayerInputComponent, TArray<class UInputAction*> inputs)
{
	PlayerInputComponent->BindAction(inputs[1], ETriggerEvent::Started, this, &UGrabComponent::GrabObject);
	PlayerInputComponent->BindAction(inputs[1], ETriggerEvent::Completed, this, &UGrabComponent::ReleaseObject);
}

void UGrabComponent::GrabObject()
{
	// ������ �浹 üũ(Object Type: PickUp)
#pragma region SweepTrace Type
	// 1. SweepTrace�� �̿��� ���
	if (currentObject == nullptr)
	{
		FHitResult hitInfo;
		FVector originLoc = player->rightHand->GetComponentLocation();
		FCollisionObjectQueryParams objectParams;
		objectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel2);

		FCollisionQueryParams params;
		params.AddIgnoredActor(player);

		bool bChecked = GetWorld()->SweepSingleByObjectType(hitInfo, originLoc, originLoc, FQuat::Identity, objectParams, FCollisionShape::MakeSphere(30), params);

		if (bChecked)
		{
			currentObject = Cast<APickUpActor>(hitInfo.GetActor());
			//UE_LOG(LogTemp, Warning, TEXT("%s(%d) - Hit Actor: %s"), *FString(__FUNCTION__), __LINE__, *hitInfo.GetActor()->GetActorNameOrLabel());

			if (currentObject != nullptr)
			{
				currentObject->OnGrabbed(player->rightHand);

				// ������ ��Ʈ�ѷ��� ���� ȿ���� �ش�.
				player->pc->PlayHapticEffect(grabHaptic, EControllerHand::Right);
			}
		}
	}
#pragma endregion

#pragma region Overlap Type
	// 2. overlap�� �̿��� ���
	//TArray<FOverlapResult> hitInfos;
	//FCollisionObjectQueryParams objectParams;
	//objectParams.AddObjectTypesToQuery(ECC_GameTraceChannel2);

	//FCollisionQueryParams params;
	//params.AddIgnoredActor(player);

	//bool bChecked = GetWorld()->OverlapMultiByObjectType(hitInfos, player->rightHand->GetComponentLocation(), FQuat::Identity, objectParams, FCollisionShape::MakeSphere(20), params);
	//// �浹 üũ ���� �ð�ȭ
	//DrawDebugSphere(GetWorld(), player->rightHand->GetComponentLocation(), 20, 30, FColor::Green, false, 2.0f, 0, 1);

	//if (bChecked)
	//{
	//	// ���� ���� ������ ��� PickUpActor ������Ʈ���� �� ���
	//	for (const FOverlapResult& hitInfo : hitInfos)
	//	{
	//		APickUpActor* pickedObj = Cast<APickUpActor>(hitInfo.GetActor());

	//		if (pickedObj != nullptr)
	//		{
	//			pickedObj->OnGrabbed(player->rightHand);
	//		}
	//	}
	//}
#pragma endregion

}

// ��ü�� ���� �Լ�
void UGrabComponent::ReleaseObject()
{
	// ���� ��� �ִ� ��ü�� �ִ��� Ȯ���Ѵ�.
	if (currentObject != nullptr)
	{
		// ������ ��Ʈ�ѷ��� ��ġ ��ȭ���� ����Ѵ�.
		FVector deltaDirection = currentLocation_rightCon - previousLocation_rightCon;

		// ������ ��Ʈ�ѷ��� ȸ�� ��ȭ���� ����Ѵ�.
		FQuat deltaRotation = currentRotation_rightCon - previousRotation_rightCon;
		//FQuat deltaRotation = currentRotation_rightCon * previousRotation_rightCon.Inverse();

		currentObject->OnReleased(deltaDirection, throwThreshold, deltaRotation.GetRotationAxis());
		currentObject = nullptr;
	}
}

