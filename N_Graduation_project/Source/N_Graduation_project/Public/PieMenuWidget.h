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

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeConstruct() override;

	float CalculateMouseAngle();
	void StandardPosition();
	void CacheFinalMouseAngle();


	UFUNCTION(BlueprintCallable)
	float GetCachedMouseAngle() const { return CachedMouseFinalAngle; }

	FVector2D InitialMousePosition; // 처음 마우스 위치
	bool bIsReferencePointSet = false; // 초기화 여부 확인용
	float CachedMouseAngle=0.2f;
	float CachedMouseFinalAngle;
	float Before_CachedMouseFinalAngle= 102.0f;

	UPROPERTY()
	TSet<FString> DeadMonsters;

	float SectorAngle = 360.0f / 7.0f; // 7개 섹터
	FString Monster;
	FString BeforeMonster;

	UPROPERTY()
	TArray<UScaleBox*> PieScaleBoxes;

	void OpenCharacter(FString DeadMonsterName);

	bool WildBoar_Ok = false;
	bool Inpermon_OK = false;
	bool SkeletonWarrior_OK = false;
	bool StoneGolem_OK = false;
	bool SkeletonArcher_OK = false;
	bool Freezard_OK = false;

};
