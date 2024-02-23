// Fill out your copyright notice in the Description page of Project Settings.


#include "PianoActor.h"
#include "Components/SceneComponent.h"
#include "Components/ChildActorComponent.h"
#include "KeyboardActor.h"


APianoActor::APianoActor()
{
	PrimaryActorTick.bCanEverTick = true;

	rootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	SetRootComponent(rootComp);

	keySounds.SetNum(8);

	// ��ü �ʺ� = �ǹ��� ���� * �ǹ��� �ʺ� + (�ǹ��� ���� - 1) * �ǹ� ������ ����
	float entireWidth = keySounds.Num() * 10 + (keySounds.Num() - 1) * keyboardTerm;

	// ���� ��ġ(y��) = ��ü �ʺ� * -0.5 + �ǹ��� �ʺ� * 0.5
	float startLocation = -0.5f * entireWidth + 10 * 0.5f;

	// ��ġ ���� = �ǹ��� �ʺ� + �ǹ� ������ ����
	float term = 10 + keyboardTerm;

	for (int32 i = 0; i < keySounds.Num(); i++)
	{
		FString keyCompName = FString::Printf(TEXT("Key_%d"), i + 1);
		UChildActorComponent* cKey = CreateDefaultSubobject<UChildActorComponent>(FName(keyCompName));
		cKey->SetupAttachment(RootComponent);
		FVector keyLocation = FVector(0, startLocation + term * i, 0);
		cKey->SetRelativeLocation(keyLocation);
		childKeys.Add(cKey);
	}

	auto bp_keyboard = ConstructorHelpers::FClassFinder<AKeyboardActor>(TEXT("/Game/Blueprints/BP_KeyboardActor"));
	if (bp_keyboard.Succeeded())
	{
		keyboardActor = bp_keyboard.Class;
	}

	if (keyboardActor != nullptr)
	{
		for (int32 i = 0; i < keySounds.Num(); i++)
		{
			// Child Actor ��� - childActorComponent�� ������ Ű���� ���͸� �Ҵ��Ѵ�.
			childKeys[i]->SetChildActorClass(keyboardActor);
		}
	}
}

void APianoActor::BeginPlay()
{
	Super::BeginPlay();

	for (int32 i = 0; i < keySounds.Num(); i++)
	{
		Cast<AKeyboardActor>(childKeys[i]->GetChildActor())->keySound = keySounds[i];
	}
}

void APianoActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

