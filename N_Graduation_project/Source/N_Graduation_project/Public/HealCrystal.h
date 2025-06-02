// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealCrystal.generated.h"

UCLASS()
class N_GRADUATION_PROJECT_API AHealCrystal: public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHealCrystal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void NomalHeal();

	UPROPERTY(BlueprintReadWrite)
		int hp = 120;
	UPROPERTY(BlueprintReadWrite)
		float StartHeal = 5.f;
	UPROPERTY(BlueprintReadWrite)
		int Heal =30; //실패시 200
	UPROPERTY(BlueprintReadWrite)
		float Limit = 15.f;
	UFUNCTION(BlueprintCallable)
		void StartCrystal(float LimitTime);

	UPROPERTY(EditAnywhere)
		AActor* BossActor;

	UFUNCTION()
		void OnCrystalTimeExpired();
	// HealCrystal.h
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Crystal")
		float HealDelay = 5.0f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Crystal")
		int32 HealAmountPerSecond = 30; //1초마다 회복

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Crystal")
		float HealInterval = 1.0f; // 1초마다 회복

	FTimerHandle CrystalExpireTimerHandle;//15초
	FTimerHandle HealLoopTimerHandle;//1초마다 회복

	bool bIsDestroyed = false;

	virtual float TakeDamage(float DamageAmount,struct FDamageEvent const& DamageEvent,class AController* EventInstigator,AActor* DamageCauser) override;
	
	void SetHP(int NewHP);
	float currentHP = 120;

	USoundWave* BossHealSound ;
	USoundWave* BossBreakSound ;
	USoundWave* BossOverTimeSound ;
};
