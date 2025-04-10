// Fill out your copyright notice in the Description page of Project Settings.


#include "PieMenuWidget.h"
#include "Components/Image.h"

void UPieMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	for (int i = 1; i <= 7; i++)
	{
		FString Name = FString::Printf(TEXT("slot%d"), i);
		if (UScaleBox* ScaleBox = Cast<UScaleBox>(GetWidgetFromName(*Name)))
		{
			UE_LOG(LogTemp, Log, TEXT("Yes ScaleBox"));
			PieScaleBoxes.Add(ScaleBox);
		}
		else {
			UE_LOG(LogTemp, Log, TEXT("No ScaleBox"));
		}
	}


}


void UPieMenuWidget::OnCollection_Implementation(const FString& DeadMonsterName)
{
	if (UScaleBox* CollectionScaleBoxes = Cast<UScaleBox>(GetWidgetFromName(TEXT("Collection_box")))) {
		//	CollectionScaleBoxes ->SetVisibility(true);
		UE_LOG(LogTemp, Warning, TEXT("OnCollection_Implementation 실행됨"));

		if (UImage* CollecterIcon = Cast<UImage>(GetWidgetFromName(TEXT("Image_265"))))
		{
			if (DeadMonsterName == "WildBoar") {
				UTexture2D* NewTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/Image/Icon/monkey_ic.monkey_ic"));
				if (NewTexture)
				{
					CollecterIcon->SetBrushFromTexture(NewTexture);
				}
			}
		}
	}
}

void UPieMenuWidget::OpenCharacter(FString DeadMonsterName)
{
	UE_LOG(LogTemp, Log, TEXT("OpenCharacter 실행됨_ %s"), *DeadMonsterName);
	// Monster 이름과 ScaleBox 인덱스 매핑
	TMap<FString, int32> PieMap = {
		{"WildBoar", 2},
		{"Inpermon", 3},
		{"lizard", 4},
		{"StoneGolem", 5},
		{"SkeletonWarrior", 6},
		{"SkeletonArcher", 7},
	};

	// 몬스터 이름이 등록되어 있다면 해당 슬롯 활성화
	if (PieMap.Contains(DeadMonsterName))
	{
		int32 Index = PieMap[DeadMonsterName];

		if (PieScaleBoxes.IsValidIndex(Index))
		{
			PieScaleBoxes[Index]->SetVisibility(ESlateVisibility::Visible);
			UE_LOG(LogTemp, Log, TEXT("%s 해금 완료 - 슬롯 %d 활성화됨"), *DeadMonsterName, Index);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("PieScaleBoxes에 유효한 인덱스가 아님: %d"), Index);
		}
		if (DeadMonsterName == "WildBoar") {
			UE_LOG(LogTemp, Log, TEXT("와일드 보어 해금 완료"));
			PieScaleBoxes[2]->SetVisibility(ESlateVisibility::Visible);
		}

		else if (DeadMonsterName == "Inpermon")
		{
			UE_LOG(LogTemp, Log, TEXT("인페르몽 해금 완료"));
		}
		else if (DeadMonsterName == "SkeletonWarrior") {}
		else if (DeadMonsterName == "StoneGolem") {}
		else if (DeadMonsterName == "SkeletonArcher") {}
		else if (DeadMonsterName == "lizard") {}

	}
}
