#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EntityWidget.generated.h"

UCLASS()
class N_GRADUATION_PROJECT_API UEntityWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// HP 업데이트
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateHealthBar(int32 NewHealth);

	
	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(BlueprintReadWrite, Category = "Entity")
	float HP;

	// 타이머 핸들
	FTimerHandle HealthTimerHandle;

	// HP를 감소시키는 함수
	void DecreaseHealth();

	UFUNCTION(BlueprintCallable, Category = "Entity")
	float GetCurrentHP() const { return HP; }

	// NativeConstruct 함수 추가
	virtual void NativeConstruct() override;

	// NativeTick 함수 추가
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
