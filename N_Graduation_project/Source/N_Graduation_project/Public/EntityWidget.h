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

	// 이름 받기
	UFUNCTION(BlueprintCallable, Category = "Entity")
	void ReceiveEntityName(const FText& NewName);

	// 속도 받기
	UFUNCTION(BlueprintCallable, Category = "Entity")
	void ReceiveEntitySpeed(int32 NewSpeed);
	
	// 현재 이름과 속도를 저장
	UPROPERTY(BlueprintReadWrite, Category = "Entity")
	FText EntityName;

	UPROPERTY(BlueprintReadWrite, Category = "Entity")
	int32 EntitySpeed;

	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(BlueprintReadWrite, Category = "Entity")
	float MaxHP;

	// 타이머 핸들
	FTimerHandle HealthTimerHandle;

	// HP를 감소시키는 함수
	void DecreaseHealth();

	UFUNCTION(BlueprintCallable, Category = "Entity")
	float GetCurrentHP() const { return MaxHP; }

	// NativeConstruct 함수 추가
	virtual void NativeConstruct() override;

	// NativeTick 함수 추가
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

};