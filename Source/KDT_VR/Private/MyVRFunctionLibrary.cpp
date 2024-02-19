// Fill out your copyright notice in the Description page of Project Settings.


#include "MyVRFunctionLibrary.h"


// ��� �׸��� �ʹ�.
// ������ �ʿ��ϴ�. -> ������ � ����
// p0 -> p1 ���� ���� �׸���.
// p1 -> p2���� ���� �׸���.
// p0 ~ p1������ ������ ���� ���� ��ġ(m1)�� ���Ѵ�. -> Lerp()
// p1 ~ p2������ ������ ���� ���� ��ġ(m2)�� ���Ѵ�. -> Lerp()
// m1 ~ m2���� ���� �׸���.
// m1 ~ m2������ ������ ���� ���� ��ġ(b0)�� ���Ѵ�.
// b0�� ��ġ ������ �迭�� �����Ѵ�.

TArray<FVector> UMyVRFunctionLibrary::DrawBezierCurve(FVector p0, FVector p1, FVector p2, float term)
{
	// ��� ���� ���� �迭 ������ �����Ѵ�.
	TArray<FVector> Points;

	for (int32 i = 0; i < term; i++)
	{
		float interval = (float)i / term;
		FVector m0 = FMath::Lerp(p0, p1, interval);
		//DrawDebugLine(GetWorld(), p0, m0, FColor::Red, false, 0, 0, 1);

		FVector m1 = FMath::Lerp(p1, p2, interval);
		//DrawDebugLine(GetWorld(), p1, m1, FColor::Red, false, 0, 0, 1);

		//DrawDebugLine(GetWorld(), m0, m1, FColor::Green, false, 0, 0, 1);
		FVector b0 = FMath::Lerp(m0, m1, interval);
		Points.Add(b0);
	}

	// ���� ��� �� ������ ��ġ ���� ��Ƴ��� �迭 Points ������ ��ȯ�Ѵ�.
	return Points;
}
