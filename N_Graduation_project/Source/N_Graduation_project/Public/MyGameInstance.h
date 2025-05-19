// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class N_GRADUATION_PROJECT_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:	
		UPROPERTY(BlueprintReadOnly)
		bool GetKey = false;

		bool WildBoar_Ok = false;
		bool Inpermon_OK = false;
		bool SkeletonWarrior_OK = false;
		bool StoneGolem_OK = false;
		bool SkeletonArcher_OK = false;
		bool Freezard_OK = false;

		// 불러오기 함수 (저장 데이터를 바탕으로 초기화)
		void InitFromSaveData(class UMySaveGame* SaveData);
};
