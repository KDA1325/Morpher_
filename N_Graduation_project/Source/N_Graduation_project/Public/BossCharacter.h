// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BossPatternManager.h"
#include "Components/WidgetComponent.h"
#include "NiagaraSystem.h"

#include "BossCharacter.generated.h"

class BossPatternManager;
class UBossWidget;

UCLASS()
class N_GRADUATION_PROJECT_API ABossCharacter: public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABossCharacter();

	void HealHP(int DamageAmount);
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
		int CurrentHP;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
		ABossPatternManager* BossPatternManager;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
		UWidgetComponent* BossHPWidgetComponent;

	virtual float TakeDamage(float DamageAmount,struct FDamageEvent const& DamageEvent,class AController* EventInstigator,AActor* DamageCauser) override;
	void SetHP(int NewHP);
	void UpdateHP();

	void DestroyAllAttachedLasers();

	void OnBossDead();
	UBossWidget* GetHUD() const;

	int BossHP=2000;


	UPROPERTY(EditAnywhere,Category = "UI")
		TSubclassOf<UUserWidget> BossClass;
	UPROPERTY(EditAnywhere,Category = "UI")
		UBossWidget* BossWidget;
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Spin")
		TSubclassOf<ABossProjectile> ProjectileClass;	
	UPROPERTY(EditDefaultsOnly,Category = "Spin")
		TSubclassOf<AActor> SpinningBP2Class;
	UPROPERTY(EditDefaultsOnly,Category = "Spin")
		TSubclassOf<AActor> SpinningBP2_2Class;

	UPROPERTY(EditAnywhere,Category="Effects")
		UNiagaraSystem* HealEffect;
	void Pattern1();
	void Pattern2();
	void ExecuteBossPattern();
	void ExecuteBossPattern2();
	bool bPhase2Started=false;

private:
	FTimerHandle LaserDelayHandle;
	FTimerHandle SpinDelayHandle;
	FTimerHandle HealDelayHandle;
	FTimerHandle MDelayHandle; 
	FTimerHandle PatternLoopHandle;
	FTimerHandle PatternLoopHandle2;
};
