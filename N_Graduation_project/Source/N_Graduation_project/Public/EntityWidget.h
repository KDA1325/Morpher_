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

	// 이름 업데이트
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetEntityName(FText NewName);

	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	class UTextBlock* EntityName;

	UPROPERTY(BlueprintReadWrite, Category = "Entity")
	float HP;

	UFUNCTION(BlueprintCallable, Category = "Entity")
	float GetHP() const { return HP; }

};
