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
	UFUNCTION(BlueprintCallable)
	void UpdateHPBar(float CurrentHP, float MaxHP);

	UFUNCTION(BlueprintCallable)
	void UpdateMana(int ManaAmount);

	UFUNCTION(BlueprintCallable)
	void ChangeIcon(const FString& MonsterName);

	UFUNCTION(BlueprintCallable)
	void UpdateSkillCooldown(float CoolTime, bool nomal, bool special);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TArray<UImage*> ManaTokenImages;

	UPROPERTY(meta = (BindWidget))
	TArray<UImage*> IconImages;

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float CoolTime;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool CanNomal;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool CanSpecial;
};
