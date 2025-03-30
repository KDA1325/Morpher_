#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EntityWidget.generated.h"

class AEntityPreset;

UCLASS()
class N_GRADUATION_PROJECT_API UEntityWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// HP 업데이트
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateHealthBar(float NewHealth);

	//UPROPERTY(BlueprintReadOnly)
	//float UpdateHP;

	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	class UProgressBar* HealthBar;

	// NativeConstruct 함수 추가
	virtual void NativeConstruct() override;

	// NativeTick 함수 추가
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY()
	AEntityPreset* EntityPreset;
};