// Fill out your copyright notice in the Description page of Project Settings.


#include "PieMenuWidget.h"
#include "Components/Image.h"

void UPieMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	/*for (int i = 1; i <= 7; i++)
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
	}*/\
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

	if (DeadMonsterName == "WildBoar") {
		UE_LOG(LogTemp, Log, TEXT("와일드 보어 해금 완료"));
		if (UImage* CollecterIcon = Cast<UImage>(GetWidgetFromName(TEXT("boar_img"))))
		{
			CollecterIcon->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else if (DeadMonsterName == "Inpermon")
	{
		UE_LOG(LogTemp, Log, TEXT("인페르몽 해금 완료"));
		if (UImage* CollecterIcon = Cast<UImage>(GetWidgetFromName(TEXT("monkey_img"))))
		{
			CollecterIcon->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else if (DeadMonsterName == "SkeletonWarrior") {
		if (UImage* CollecterIcon = Cast<UImage>(GetWidgetFromName(TEXT("Skeleton1_img"))))
		{
			CollecterIcon->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else if (DeadMonsterName == "StoneGolem") {
		if (UImage* CollecterIcon = Cast<UImage>(GetWidgetFromName(TEXT("Golem_img"))))
		{
			CollecterIcon->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else if (DeadMonsterName == "SkeletonArcher") {
		if (UImage* CollecterIcon = Cast<UImage>(GetWidgetFromName(TEXT("Skeleton2_img"))))
		{
			CollecterIcon->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else if (DeadMonsterName == "lizard") {
		if (UImage* CollecterIcon = Cast<UImage>(GetWidgetFromName(TEXT("lizard_img"))))
		{
			CollecterIcon->SetVisibility(ESlateVisibility::Visible);
		}
	}
}
