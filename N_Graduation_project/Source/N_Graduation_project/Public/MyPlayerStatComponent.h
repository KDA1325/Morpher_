#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyPlayerStatComponent.generated.h"

class AN_Graduation_projectCharacter;
class UMyCharacterWidget;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class N_GRADUATION_PROJECT_API UMyPlayerStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMyPlayerStatComponent();

protected:
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;
public:
	void SetHP(float NewHP);
	void SetMaxHp(int MaxHp);
	void ApplyDamage(float DamageAmount);
	void SetMana(int NewMana);
	void UseMana(int ManaAmount);
	void TransformToEntity(FString Name, int HP, int Mana);
	void RegenerateMana();

private:
	void UpdateHUD();//위젯값을 연결

public:
	UPROPERTY()
	AN_Graduation_projectCharacter* OwnerPlayer;

	UPROPERTY()
	UMyCharacterWidget* HUDWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float CurrentHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	int CurrentMana;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	int NewMaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	int PastMaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	FString MonsterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> HUDClass;

	FTimerHandle ManaRegenTimerHandle;
};
