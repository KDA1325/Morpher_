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


	//	UE_LOG(LogTemp, Log, TEXT("현재 마우스 각도: %f / 회전 적용: %f"), CachedMouseAngle, CachedMouseFinalAngle);

}

void UPieMenuWidget::StandardPosition()
{
	//파이 메뉴 시작 시 마우스 위치를 기준점으로 저장
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC && UWidgetLayoutLibrary::GetMousePositionScaledByDPI(PC, InitialMousePosition.X, InitialMousePosition.Y))
	{
		// 마우스 위치를 DPI 기준으로 가져오고 성공했다면 저장
		bIsReferencePointSet = true;
		// 기준점이 정상적으로 설정되었음을 표시
	}
}

float UPieMenuWidget::CalculateMouseAngle()
{
	// 현재 마우스 위치와 기준점을 비교하여 각도 계산
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC || !bIsReferencePointSet) return 0.0f;

	FVector2D CurrentMousePosition;
	if (!UWidgetLayoutLibrary::GetMousePositionScaledByDPI(PC, CurrentMousePosition.X, CurrentMousePosition.Y))
		return 0.0f;

	// 방향 벡터 계산 (현재 위치 - 기준점)
	FVector2D Direction = CurrentMousePosition - InitialMousePosition;
	float Distance = Direction.Size();

	const float IgnoreDistance = 50.0f;	// 일정 거리 이내(중앙에 가까운 클릭)는 무시 처리
	if (Distance < IgnoreDistance)
	{
		return -1.0f; // 무시
	}

	float AngleRad = FMath::Atan2(Direction.Y, Direction.X);// 아크탄젠트로 각도 계산 (라디안)
	float AngleDeg = FMath::RadiansToDegrees(AngleRad);	// 각도를 도 단위로 변환

	if (AngleDeg < 0.0f)
		AngleDeg += 360.0f;	// 음수 각도일 경우 360도 기준으로 양수로 변환

	return AngleDeg;
}

void UPieMenuWidget::CacheFinalMouseAngle()
{
	CachedMouseFinalAngle = CachedMouseAngle;
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

	//	UE_LOG(LogTemp, Warning, TEXT("최종 선택된 각도: %f"), CachedMouseFinalAngle);
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
	else if (DeadMonsterName == "Freezard") {
		if (UImage* CollecterIcon = Cast<UImage>(GetWidgetFromName(TEXT("lizard_img"))))
		{
			CollecterIcon->SetVisibility(ESlateVisibility::Visible);
		}
	}
}