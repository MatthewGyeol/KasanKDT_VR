// Fill out your copyright notice in the Description page of Project Settings.


#include "VRPlayer.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "MotionControllerComponent.h"
#include "Components/TextRenderComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


AVRPlayer::AVRPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	cameraComp->SetupAttachment(RootComponent);

	headMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadMesh"));
	headMesh->SetupAttachment(cameraComp);

	leftMotion = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("Left Motion Controller"));
	leftMotion->SetupAttachment(RootComponent);
	leftMotion->MotionSource = FName("Left");

	leftHand = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Left Hand Mesh"));
	leftHand->SetupAttachment(leftMotion);
	leftHand->SetRelativeRotation(FRotator(0, 180, -270));

	leftLog = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Left Log"));
	leftLog->SetupAttachment(leftMotion);
	leftLog->SetRelativeLocation(FVector(20, 0, 0));
	leftLog->SetRelativeRotation(FRotator(90, 180, 0));
	leftLog->SetHorizontalAlignment(EHTA_Center);
	leftLog->SetVerticalAlignment(EVRTA_TextCenter);
	leftLog->SetWorldSize(20);
	leftLog->SetTextRenderColor(FColor::Yellow);

	rightMotion = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("Right Motion Controller"));
	rightMotion->SetupAttachment(RootComponent);
	rightMotion->MotionSource = FName("Right");

	rightHand = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Right Hand Mesh"));
	rightHand->SetupAttachment(rightMotion);
	rightHand->SetRelativeRotation(FRotator(0, 0, 90));

	rightLog = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Right Log"));
	rightLog->SetupAttachment(rightMotion);
	rightLog->SetRelativeLocation(FVector(20, 0, 0));
	rightLog->SetRelativeRotation(FRotator(90, 180, 0));
	rightLog->SetHorizontalAlignment(EHTA_Center);
	rightLog->SetVerticalAlignment(EVRTA_TextCenter);
	rightLog->SetWorldSize(20);
	rightLog->SetTextRenderColor(FColor::Yellow);
}

void AVRPlayer::BeginPlay()
{
	Super::BeginPlay();

	// Input Mapping Context ������ �Է� ���� �ý��ۿ� ����ϴ� ������ �����Ѵ�.
	APlayerController* pc = GetController<APlayerController>();
	if (pc != nullptr)
	{
		UEnhancedInputLocalPlayerSubsystem* subsys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());

		if (subsys != nullptr)
		{
			subsys->AddMappingContext(vrMapping, 0);
		}

	}

}

void AVRPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVRPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (enhancedInputComponent != nullptr)
	{
		// �Է� ������Ʈ�� RightTriggerTouch() �Լ��� �����Ѵ�.(����� ��, ���� ��)
		enhancedInputComponent->BindAction(rightTriggerTouch, ETriggerEvent::Started, this, &AVRPlayer::RightTriggerTouch);
		enhancedInputComponent->BindAction(rightTriggerTouch, ETriggerEvent::Completed, this, &AVRPlayer::RightTriggerTouch);
		enhancedInputComponent->BindAction(rightTriggerPress, ETriggerEvent::Started, this, &AVRPlayer::RightTriggerPress);
		enhancedInputComponent->BindAction(rightTriggerPress, ETriggerEvent::Completed, this, &AVRPlayer::RightTriggerPress);
		enhancedInputComponent->BindAction(rightTriggerValue, ETriggerEvent::Triggered, this, &AVRPlayer::RightTriggerValue);
		enhancedInputComponent->BindAction(rightTriggerValue, ETriggerEvent::Completed, this, &AVRPlayer::RightTriggerValue);
	}
}

void AVRPlayer::RightTriggerTouch(const FInputActionValue& val)
{
	// �Է¹��� ���� ������ �α׿� ����Ѵ�.
	FString result = val.Get<bool>() == true ? FString("True") : FString("False");
	//rightLog->SetText(FText::FromString(FString::Printf(TEXT("Right Index touch: %s"), *result)));
}

void AVRPlayer::RightTriggerPress(const FInputActionValue& val)
{
	FString result = val.Get<bool>() == true ? FString("True") : FString("False");
	//rightLog->SetText(FText::FromString(FString::Printf(TEXT("Right Index Pressed: %s"), *result)));
}

void AVRPlayer::RightTriggerValue(const FInputActionValue& val)
{
	float pressed = val.Get<float>();
	rightLog->SetText(FText::FromString(FString::Printf(TEXT("Right Index Pressed: %.3f"), pressed)));
}

