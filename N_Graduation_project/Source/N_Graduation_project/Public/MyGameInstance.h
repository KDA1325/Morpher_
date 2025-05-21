// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MySaveGame.h"
#include "MyGameInstance.generated.h"

/**
 *
 */
UCLASS()
class N_GRADUATION_PROJECT_API UMyGameInstance: public UGameInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		bool GetKey = false;
	UPROPERTY(BlueprintReadWrite)
		FString SaveRoomName;
	UPROPERTY(BlueprintReadWrite)
		FVector SaveLocation;

	bool WildBoar_Ok = false;
	bool Inpermon_OK = false;
	bool SkeletonWarrior_OK = false;
	bool StoneGolem_OK = false;
	bool SkeletonArcher_OK = false;
	bool Freezard_OK = false;

	FString CurrentPlayerCharacter="PlayerCharacter";
	float PlayerFullHP=150;
	float PlayerFinalAngle=102.f
		;
	//게임 시작 시 자동 로드
	void Init();

	// 세이브 / 로드 함수
	UFUNCTION(BlueprintCallable)
		void SaveGame();
	UFUNCTION(BlueprintCallable)
		void LoadGame();

	// 내부 사용 함수
	void InitFromSaveData(UMySaveGame* SaveData);
	void SaveToSaveData(UMySaveGame* SaveData);

};
