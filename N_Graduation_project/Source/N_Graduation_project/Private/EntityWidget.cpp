#include "EntityWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "EntityPreset.h"

// NativeConstruct에서 타이머 설정
void UEntityWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

// HP 업데이트
void UEntityWidget::UpdateHealthBar(float NewHealth)
{
	if (HealthBar)
	{
		float HealthRatio = EntityPreset->GetHPRatio();
		// ProgressBar에 클램프된 체력 비율 설정 (0 ~ 1 사이)
		HealthBar->SetPercent(FMath::Clamp(HealthRatio, 0.0f, 1.0f));
		UE_LOG(LogTemp, Log, TEXT("banana UpdateHealthBar: %f"), HealthRatio);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("banana no HealthBar"));

	}
}

// NativeTick 함수에서 HP 감소 확인
void UEntityWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}
