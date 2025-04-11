// Fill out your copyright notice in the Description page of Project Settings.


#include "PieMenuWidget.h"
#include "Components/Image.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

void UPieMenuWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	float CalculatedAngle = CalculateMouseAngle();

	if (CalculatedAngle >= 0.0f) // 유효한 경우만 갱신
	{
		CachedMouseAngle = CalculatedAngle;
	}


	UE_LOG(LogTemp, Log, TEXT("현재 마우스 각도: %f / 회전 적용: %f"), CachedMouseAngle, CachedMouseFinalAngle);

}

void UPieMenuWidget::SetInitialMousePosition()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC && UWidgetLayoutLibrary::GetMousePositionScaledByDPI(PC, InitialMousePosition.X, InitialMousePosition.Y))
	{
		bHasInitialMousePosition = true;
		UE_LOG(LogTemp, Log, TEXT("초기 마우스 위치 저장됨: %s"), *InitialMousePosition.ToString());
	}
}

float UPieMenuWidget::CalculateMouseAngle()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC || !bHasInitialMousePosition) return 0.0f;

	FVector2D CurrentMousePosition;
	if (!UWidgetLayoutLibrary::GetMousePositionScaledByDPI(PC, CurrentMousePosition.X, CurrentMousePosition.Y))
		return 0.0f;

	FVector2D Direction = CurrentMousePosition - InitialMousePosition;
	float Distance = Direction.Size();

	const float IgnoreDistance = 50.0f;
	if (Distance < IgnoreDistance)
	{
		return -1.0f; // 중심 안이면 무시
	}


	float AngleRad = FMath::Atan2(Direction.Y, Direction.X);
	float AngleDeg = FMath::RadiansToDegrees(AngleRad);

	if (AngleDeg < 0.0f)
		AngleDeg += 360.0f;

	return AngleDeg;
}

void UPieMenuWidget::CacheFinalMouseAngle()
{
	CachedMouseFinalAngle = CachedMouseAngle;
	// CachedMouseFinalAngle 기준으로 가장 가까운 섹터 각도 하드코딩
	if (CachedMouseFinalAngle <= 25.0f)
	{
		CachedMouseFinalAngle = 0.0f;
	}
	else if (CachedMouseFinalAngle <= 77.0f)
	{
		CachedMouseFinalAngle = 309.0f;
		//CachedMouseFinalAngle = 51.0f;
	}
	else if (CachedMouseFinalAngle <= 129.0f)
	{
		CachedMouseFinalAngle = 257.0f;

//		CachedMouseFinalAngle = 103.0f;
	}
	else if (CachedMouseFinalAngle <= 181.0f)
	{
		CachedMouseFinalAngle = 206.0f;

//		CachedMouseFinalAngle = 154.0f;
	}
	else if (CachedMouseFinalAngle <= 233.0f)
	{
		CachedMouseFinalAngle = 154.0f;

//		CachedMouseFinalAngle = 206.0f;
	}
	else if (CachedMouseFinalAngle <= 285.0f)
	{
		CachedMouseFinalAngle = 103.0f;

		//CachedMouseFinalAngle = 257.0f;
	}
	else if (CachedMouseFinalAngle <= 337.0f)
	{
		CachedMouseFinalAngle = 51.0f;
//		CachedMouseFinalAngle = 309.0f;
	}
	else
	{
		
	}

	UE_LOG(LogTemp, Warning, TEXT("최종 선택된 각도: %f"), CachedMouseFinalAngle);
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