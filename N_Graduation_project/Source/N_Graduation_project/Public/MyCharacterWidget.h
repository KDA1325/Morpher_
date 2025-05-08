#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "MyCharacterWidget.generated.h"

UCLASS()
class N_GRADUATION_PROJECT_API UMyCharacterWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FString SkillName;

	UFUNCTION(BlueprintCallable)
	void UpdateHPBar(float CurrentHP, float MaxHP);

	UFUNCTION(BlueprintCallable)
	void UpdateMana(int ManaAmount);

	UFUNCTION(BlueprintCallable)
	void ChangeIcon(const FString& MonsterName);

	UFUNCTION(BlueprintCallable)
	void SetSkillIcon();

	UFUNCTION(BlueprintCallable)
	void UpdateNomalSkillCooldown(float CoolTime, bool nomal, bool special);

	UFUNCTION(BlueprintCallable)
	void UpdateSpecialSkillCooldown(float CoolTime, bool nomal, bool special);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	TArray<UImage*> ManaTokenImages;

	UPROPERTY(meta = (BindWidget))
	TArray<UImage*> IconImages;

public:

	UPROPERTY(BlueprintReadOnly, Category = "SkillCoolTime")
	bool CanNomal;

	UPROPERTY(BlueprintReadOnly, Category = "SkillCoolTime")
	bool CanSpecial;

	bool SkeletonGuard;

	float Percent;
	float PassedTimeNomal = 0.0f;
	float PassedTimeSpecial = 0.0f;

	float SkillCoolTimeNomal = 0.0f;
	float SkillCoolTimeSpecial = 0.0f;

private:
	UPROPERTY()
	UMaterialInstanceDynamic* CooldownMID1;

	UPROPERTY()
	UMaterialInstanceDynamic* CooldownMID2;
public:
	UFUNCTION(BlueprintNativeEvent)
	void OnCollection(const FString& DeadMonsterName);

	virtual void OnCollection_Implementation(const FString& DeadMonsterName);

};
