// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"


APickUpActor::APickUpActor()
{
	PrimaryActorTick.bCanEverTick = true;

	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	SetRootComponent(boxComp);
	//boxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	boxComp->SetCollisionProfileName(FName("PickUpActor"));
	boxComp->SetBoxExtent(FVector(50));
	boxComp->SetWorldScale3D(FVector(0.1f, 0.1f, 0.4f));
	boxComp->SetSimulatePhysics(true);
	boxComp->SetEnableGravity(true);

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh Component"));
	meshComp->SetupAttachment(RootComponent);
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	meshComp->SetRelativeLocation(FVector(0, 0, -50));

}

void APickUpActor::BeginPlay()
{
	Super::BeginPlay();

}

void APickUpActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// �ڱ� �ڽ��� handMeshComp�� ������Ű�� �Լ�
void APickUpActor::OnGrabbed(USkeletalMeshComponent* handMeshComp)
{
	boxComp->SetSimulatePhysics(false);

	// 1. ���� ����� ���� ��ġ ��(���� ��ǥ ����)�� �״�� �����ϸ鼭 ���̵��� �����Ѵ�.
	//FAttachmentTransformRules attachRules = FAttachmentTransformRules::KeepWorldTransform;
	//AttachToComponent(handMeshComp, attachRules);

	// 2. �� �޽��� ���� ��ġ�� ���缭 �ڽ��� �����Ѵ�.
	FAttachmentTransformRules attachRules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;

	AttachToComponent(handMeshComp, attachRules, FName("GrabPoint"));
	SetActorRelativeLocation(offsetLocation);
	SetActorRelativeRotation(offsetRotation);
}

void APickUpActor::OnReleased(FVector deltaDir, float throwThreshold, FVector deltaRot)
{
	// 1. Ư�� ���ͷκ��� �ڽ��� �и��Ѵ�.
	FDetachmentTransformRules detachRules = FDetachmentTransformRules::KeepWorldTransform;
	DetachFromActor(detachRules);

	// 2. ������ ��ü�� ���� ȿ���� ���ش�.
	boxComp->SetSimulatePhysics(true);

	// 3. ����ڰ� �ֵθ� �������� ��ü�� ���� ���Ѵ�.
	// �ʿ� ��� : ����, ���� ũ��

	// ����, ��ġ ��ȭ���� �Ӱ������� ũ�ٸ�...
	if (deltaDir.Length() > throwThreshold)
	{
		boxComp->AddImpulse(deltaDir * throwPower);
		boxComp->AddTorqueInDegrees(deltaRot * torquePower);
	}

}

