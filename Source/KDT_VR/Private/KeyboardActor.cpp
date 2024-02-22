// Fill out your copyright notice in the Description page of Project Settings.


#include "KeyboardActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"


AKeyboardActor::AKeyboardActor()
{
	PrimaryActorTick.bCanEverTick = false;

	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	SetRootComponent(boxComp);
	boxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	boxComp->SetCollisionObjectType(ECC_WorldDynamic);
	boxComp->SetCollisionResponseToAllChannels(ECR_Overlap);
	boxComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	boxComp->SetGenerateOverlapEvents(true);
	boxComp->SetBoxExtent(FVector(50));

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	meshComp->SetupAttachment(RootComponent);
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	meshComp->SetRelativeLocation(FVector(0, 0, -50));
}

void AKeyboardActor::BeginPlay()
{
	Super::BeginPlay();
	
	// ���� ��Ƽ����� ��ü
	keyMat = UMaterialInstanceDynamic::Create(meshComp->GetMaterial(0), this);
	meshComp->SetMaterial(0, keyMat);
	SetOriginalColor();

	// �浹 �̺�Ʈ �����ϱ�
	boxComp->OnComponentBeginOverlap.AddDynamic(this, &AKeyboardActor::OnHitKey);
}

void AKeyboardActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AKeyboardActor::OnHitKey(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// ������ ������ �÷����Ѵ�.
	UGameplayStatics::PlaySound2D(GetWorld(), keySound);

	// ��� �Һ��� ���Դٰ� 0.5�� �ڿ� �ٽ� ���� �������� ���ư���.
	keyMat->SetVectorParameterValue(FName("HitColor"), FVector4(0.174f, 1.0f, 0.086f, 1.0f));
	keyMat->SetScalarParameterValue(FName("Bloom"), 3.0f);

	FTimerHandle resetColorHandler;
	GetWorldTimerManager().ClearTimer(resetColorHandler);
	GetWorldTimerManager().SetTimer(resetColorHandler, this, &AKeyboardActor::SetOriginalColor, 0.5f, false);
}

void AKeyboardActor::SetOriginalColor()
{
	keyMat->SetVectorParameterValue(FName("HitColor"), FVector4(1.0f, 1.0f, 1.0f, 1.0f));
	keyMat->SetScalarParameterValue(FName("Bloom"), 1.0f);
}

