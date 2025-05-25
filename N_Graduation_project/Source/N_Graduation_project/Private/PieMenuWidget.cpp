// Fill out your copyright notice in the Description page of Project Settings.


#include "PieMenuWidget.h"
#include "Components/Image.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "MyGameInstance.h"
void UPieMenuWidget::NativeTick(const FGeometry& MyGeometry,float InDeltaTime)
{
	Super::NativeTick(MyGeometry,InDeltaTime);
	float CalculatedAngle = CalculateMouseAngle();

	if(CalculatedAngle > 0.0f) // 유효한 경우만 갱신
	{
		CachedMouseAngle = CalculatedAngle;
	}

	//	UE_LOG(LogTemp, Log, TEXT("현재 마우스 각도: %f / 회전 적용: %f"), CachedMouseAngle, CachedMouseFinalAngle);
	//CachedMouseFinalAngle = 103.0f
}
void UPieMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	auto* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if(!MyGameInstance) return;

	// 파이 메뉴 열릴 때 초기 상태를 플레이어로 설정
	CachedMouseFinalAngle = 102.0f;
	Monster = "PlayerCharacter";
	BeforeMonster = "PlayerCharacter";
}
void UPieMenuWidget::StandardPosition()
{
	//파이 메뉴 시작 시 마우스 위치를 기준점으로 저장
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(),0);
	if(PC && UWidgetLayoutLibrary::GetMousePositionScaledByDPI(PC,InitialMousePosition.X,InitialMousePosition.Y))
	{
		// 마우스 위치를 DPI 기준으로 가져오고 성공했다면 저장
		bIsReferencePointSet = true;
		// 기준점이 정상적으로 설정되었음을 표시
	}
}

float UPieMenuWidget::CalculateMouseAngle()
{
	// 현재 마우스 위치와 기준점을 비교하여 각도 계산
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(),0);
	if(!PC || !bIsReferencePointSet) return 0.0f;

	FVector2D CurrentMousePosition;
	if(!UWidgetLayoutLibrary::GetMousePositionScaledByDPI(PC,CurrentMousePosition.X,CurrentMousePosition.Y))
		return 0.0f;

	// 방향 벡터 계산 (현재 위치 - 기준점)
	FVector2D Direction = CurrentMousePosition - InitialMousePosition;
	float Distance = Direction.Size();

	const float IgnoreDistance = 70.0f;	// 일정 거리 이내(중앙에 가까운 클릭)는 무시 처리
	if(Distance < IgnoreDistance)
	{
		return -1.0f; // 무시
	}

	float AngleRad = FMath::Atan2(Direction.Y,Direction.X);// 아크탄젠트로 각도 계산 (라디안)
	float AngleDeg = FMath::RadiansToDegrees(AngleRad);	// 각도를 도 단위로 변환

	if(AngleDeg < 0.0f)
		AngleDeg += 360.0f;	// 음수 각도일 경우 360도 기준으로 양수로 변환

	return AngleDeg;
}

void UPieMenuWidget::CacheFinalMouseAngle()
{
	auto* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if(!MyGameInstance) return;

	if(CachedMouseAngle < 0.2f) // -1.0f 등 무시 상태면 변신 처리 중단
	{
		UE_LOG(LogTemp,Warning,TEXT("원 내 클릭 무시, 변신 처리 안함"));
		return;
	}

	CachedMouseFinalAngle = CachedMouseAngle;


	if(CachedMouseFinalAngle <= 25.0f)
	{
		if(MyGameInstance->SkeletonWarrior_OK)
		{
			CachedMouseFinalAngle = -2.0f;
			Before_CachedMouseFinalAngle = CachedMouseFinalAngle;

			UE_LOG(LogTemp,Log,TEXT("전사 선택"));

			BeforeMonster = "SkeletonWarrior";
		} else {
			CachedMouseFinalAngle = Before_CachedMouseFinalAngle;

		}
	} else if(CachedMouseFinalAngle <= 77.0f)
	{
		if(MyGameInstance->StoneGolem_OK)
		{
			CachedMouseFinalAngle = 306.0f;
			Before_CachedMouseFinalAngle = CachedMouseFinalAngle;
			UE_LOG(LogTemp,Log,TEXT("골렘 선택"));

			BeforeMonster = "StoneGolem";
			//CachedMouseFinalAngle = 51.0f;
		} else {
			CachedMouseFinalAngle = Before_CachedMouseFinalAngle;

		}
	} else if(CachedMouseFinalAngle <= 129.0f)
	{
		if(MyGameInstance->Freezard_OK)
		{
			CachedMouseFinalAngle = 257.0f;
			Before_CachedMouseFinalAngle = CachedMouseFinalAngle;
			UE_LOG(LogTemp,Log,TEXT("프리자드 선택"));

			BeforeMonster = "Freezard";
			//		CachedMouseFinalAngle = 103.0f;
		} else {
			CachedMouseFinalAngle = Before_CachedMouseFinalAngle;

		}
	} else if(CachedMouseFinalAngle <= 181.0f)
	{
		if(MyGameInstance->Inpermon_OK)
		{
			CachedMouseFinalAngle = 206.0f;
			Before_CachedMouseFinalAngle = CachedMouseFinalAngle;

			UE_LOG(LogTemp,Log,TEXT("인페르몽 선택"));
			BeforeMonster = "Inpermon";
			//		CachedMouseFinalAngle = 154.0f;
		} else {
			CachedMouseFinalAngle = Before_CachedMouseFinalAngle;

		}
	} else if(CachedMouseFinalAngle <= 233.0f)
	{
		if(MyGameInstance->WildBoar_Ok)
		{
			CachedMouseFinalAngle = 154.0f;
			Before_CachedMouseFinalAngle = CachedMouseFinalAngle;
			UE_LOG(LogTemp,Log,TEXT("와일드 보어 선택"));

			BeforeMonster = "WildBoar";
			//	CachedMouseFinalAngle = 206.0f;
		} else {
			CachedMouseFinalAngle = Before_CachedMouseFinalAngle;

		}

	} else if(CachedMouseFinalAngle <= 285.0f)
	{
		if(MyGameInstance->Player_Ok)
		{
			UE_LOG(LogTemp,Log,TEXT("플레이어 선택"));

			CachedMouseFinalAngle = 102.0f;
			Before_CachedMouseFinalAngle = CachedMouseFinalAngle;

			BeforeMonster = "PlayerCharacter";
			//CachedMouseFinalAngle = 257.0f;
		} else {
			CachedMouseFinalAngle = Before_CachedMouseFinalAngle;

		}
	} 
	else if(CachedMouseFinalAngle <= 337.0f)
	{
		if(MyGameInstance->SkeletonArcher_OK)
		{
			UE_LOG(LogTemp,Log,TEXT("궁수 선택"));

			CachedMouseFinalAngle = 51.0f;
			Before_CachedMouseFinalAngle = CachedMouseFinalAngle;

			BeforeMonster = "SkeletonArcher";
			//CachedMouseFinalAngle = 309.0f;
		} else {
			CachedMouseFinalAngle = Before_CachedMouseFinalAngle;

		}
	} else
	{
		CachedMouseFinalAngle = Before_CachedMouseFinalAngle;
		Monster = BeforeMonster;

	}
	Monster = BeforeMonster;
}


void UPieMenuWidget::OpenCharacter(FString DeadMonsterName)
{
	UE_LOG(LogTemp,Log,TEXT("OpenCharacter 실행됨_ %s"),*DeadMonsterName);
	auto* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if(!MyGameInstance) return;
	if(DeadMonsterName == "WildBoar") {
		UE_LOG(LogTemp,Log,TEXT("와일드 보어 해금 완료"));
		MyGameInstance->WildBoar_Ok = true;
	} 
	else if(DeadMonsterName == "Inpermon")
	{
		UE_LOG(LogTemp,Log,TEXT("인페르몽 해금 완료"));
		MyGameInstance->Inpermon_OK = true;
	} 
	else if(DeadMonsterName == "SkeletonWarrior") {
		MyGameInstance->SkeletonWarrior_OK = true;
	} 
	else if(DeadMonsterName == "StoneGolem") {
			MyGameInstance->StoneGolem_OK = true;
	} 
	else if(DeadMonsterName == "SkeletonArcher") {
			MyGameInstance->SkeletonArcher_OK = true;
	} 
	else if(DeadMonsterName == "Freezard") {
			MyGameInstance->Freezard_OK = true;
	}
}

void UPieMenuWidget::InitUnlockIcons(){
	auto* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if(!MyGameInstance) return;

	if(MyGameInstance->WildBoar_Ok)
	{
		if(UImage* Icon = Cast<UImage>(GetWidgetFromName(TEXT("boar_img"))))
			Icon->SetVisibility(ESlateVisibility::Visible);
	}

	if(MyGameInstance->Inpermon_OK)
	{
		if(UImage* Icon = Cast<UImage>(GetWidgetFromName(TEXT("monkey_img"))))
			Icon->SetVisibility(ESlateVisibility::Visible);
	}

	if(MyGameInstance->SkeletonWarrior_OK)
	{
		if(UImage* Icon = Cast<UImage>(GetWidgetFromName(TEXT("Skeleton1_img"))))
			Icon->SetVisibility(ESlateVisibility::Visible);
	}
	
	if(MyGameInstance->StoneGolem_OK)
	{
		if(UImage* Icon = Cast<UImage>(GetWidgetFromName(TEXT("Golem_img"))))
			Icon->SetVisibility(ESlateVisibility::Visible);
	}	
	if(MyGameInstance->SkeletonArcher_OK)
	{
		if(UImage* Icon = Cast<UImage>(GetWidgetFromName(TEXT("Skeleton2_img"))))
			Icon->SetVisibility(ESlateVisibility::Visible);
	}	
	if(MyGameInstance->Freezard_OK)
	{
		if(UImage* Icon = Cast<UImage>(GetWidgetFromName(TEXT("lizard_img"))))
			Icon->SetVisibility(ESlateVisibility::Visible);
	}

}