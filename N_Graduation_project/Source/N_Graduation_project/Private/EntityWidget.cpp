#include "EntityWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"

// HP 업데이트
void UEntityWidget::UpdateHealthBar(int32 NewHealth)
{
	if (HealthBar)
	{
		this->HP = NewHealth;
		float MaxHealth = 100.0f; // 여기를 동적으로 처리하도록 개선 가능
		HealthBar->SetPercent(FMath::Clamp(NewHealth / MaxHealth, 0.0f, 1.0f));
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Health updated: %d"), NewHealth));
	}
}


// HP 감소 함수
void UEntityWidget::DecreaseHealth()
{
	if (HP > 0)
	{
		HP -= 1;
		UpdateHealthBar(HP); // HP를 감소시키고 UI 갱신
	}
	else
	{
		// HP가 0 이하일 때의 처리 (예: HP가 다 떨어지면 타이머 멈추기)
		GetWorld()->GetTimerManager().ClearTimer(HealthTimerHandle);
	}
}

// NativeConstruct에서 타이머 설정
void UEntityWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 타이머 설정 (1초마다 HP 감소)
	GetWorld()->GetTimerManager().SetTimer(HealthTimerHandle, this, &UEntityWidget::DecreaseHealth, 1.0f, true);
}

// NativeTick 함수에서 HP 감소 확인
void UEntityWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{

	Super::NativeTick(MyGeometry, InDeltaTime);

	// Tick에서 HP 감소 확인
	if (HP > 0)
	{
		HP -= 1;
		UpdateHealthBar(HP); // HP를 감소시키고 UI 갱신
	}
	else
	{
		// HP가 0 이하일 때의 처리 (예: HP가 다 떨어지면 타이머 멈추기)
		GetWorld()->GetTimerManager().ClearTimer(HealthTimerHandle);
	}
}
