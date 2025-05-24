// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BossPatternManager.h"
#include "Components/WidgetComponent.h"

#include "BossCharacter.generated.h"

class BossPatternManager;
class UBossWidget;

UCLASS()
class N_GRADUATION_PROJECT_API ABossCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABossCharacter();

	UPROPERTY(BlueprintReadWrite)
		bool SkillStart;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
	ABossPatternManager* BossPatternManager;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
		UWidgetComponent* BossHPWidgetComponent;
	void Pattern1();
	virtual float TakeDamage(float DamageAmount,struct FDamageEvent const& DamageEvent,class AController* EventInstigator,AActor* DamageCauser) override;
	void SetHP(int NewHP);
	void UpdateHP();
	void OnBossDead();
	UBossWidget* GetHUD() const;

	int BossHP=2000;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	int CurrentHP;

	UPROPERTY(EditAnywhere,Category = "UI")
		TSubclassOf<UUserWidget> BossClass;
	UPROPERTY(EditAnywhere,Category = "UI")
		UBossWidget* BossWidget;


//레이저
	void SpawnAndAttachLasers();

	UPROPERTY(EditDefaultsOnly,Category = "Laser")
		TSubclassOf<AActor> LaserBPClass;

	UPROPERTY(EditDefaultsOnly,Category = "Laser")
		int32 LaserDamage = 30;

	UPROPERTY(EditDefaultsOnly,Category = "Laser")
		TArray<FName> LaserSocketNames;
};
