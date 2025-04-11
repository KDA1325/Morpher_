// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/ScaleBox.h"
#include "PieMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class N_GRADUATION_PROJECT_API UPieMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY()
	TSet<FString> DeadMonsters;

	UPROPERTY()
	TArray<UScaleBox*> PieScaleBoxes;

	UFUNCTION(BlueprintNativeEvent)
	void OnCollection(const FString& DeadMonsterName);

	virtual void OnCollection_Implementation(const FString& DeadMonsterName);

	void OpenCharacter(FString DeadMonsterName);
};
