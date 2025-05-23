
#include "BossWidget.h"
#include "Components/ProgressBar.h"
// HP 업데이트

void UBossWidget::UpdateHPBar(float CurrentHP)
{
	if(UProgressBar* BSHealthBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("ProgressBar_55"))))
	{
		float Percent = CurrentHP / 2000.0f;
		BSHealthBar->SetPercent(FMath::Clamp(Percent,0.0f,1.0f));
		UE_LOG(LogTemp,Log,TEXT("Boss CurrentHP: %f, Percent: %f"),CurrentHP,Percent);
	}
	UE_LOG(LogTemp,Log,TEXT("Boss CurrentHP: %f"),CurrentHP);

}
