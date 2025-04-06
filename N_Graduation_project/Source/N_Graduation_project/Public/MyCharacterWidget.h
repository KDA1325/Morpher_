// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
//#include "PlayerSkillComponent.h"
#include "MyCharacterWidget.generated.h"

/**
 * 
 */
UCLASS()
class N_GRADUATION_PROJECT_API UMyCharacterWidget : public UUserWidget
{
	GENERATED_BODY()

public:	
	UFUNCTION(BlueprintCallable)
	void CollTimeData(float time, bool IsNomalCool, bool IsSpecialCool);

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	float CoolTime;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	bool bIsNomalCool;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	bool bIsSpecialCool;

};
