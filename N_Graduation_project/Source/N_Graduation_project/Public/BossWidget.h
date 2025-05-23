// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "BossWidget.generated.h"

/**
 * 
 */
UCLASS()
class N_GRADUATION_PROJECT_API UBossWidget : public UUserWidget
{
	GENERATED_BODY()
public:
		UFUNCTION(BlueprintCallable,Category = "UI")
		void UpdateHPBar(float NewHealth);

	//UPROPERTY(BlueprintReadWrite,Category = "UI",meta = (BindWidget))
	//	class UProgressBar* HealthBar;

};
