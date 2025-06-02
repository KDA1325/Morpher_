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
	UPROPERTY(BlueprintReadOnly)
		bool GetKey2 = false;
	UPROPERTY(BlueprintReadWrite)
		bool BossClear = false;
	UPROPERTY(BlueprintReadWrite)
		FName SaveRoomName;
	UPROPERTY(BlueprintReadWrite)
		FVector SaveLocation;

	//bool Player_Ok = true;
	//bool WildBoar_Ok = false;
	//bool Inpermon_OK = false;
	//bool SkeletonWarrior_OK = false;
	//bool StoneGolem_OK = false;
	//bool SkeletonArcher_OK = false;
	//bool Freezard_OK = false;

	bool Player_Ok = true;
	bool WildBoar_Ok = true;
	bool Inpermon_OK = true;
	bool SkeletonWarrior_OK = true;
	bool StoneGolem_OK = true;
	bool SkeletonArcher_OK = true;
	bool Freezard_OK = true;

	float PlayerFullHP=150;
	float PlayerFinalAngle=102.f;
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
	//void OnStreamLevelLoaded();

	UPROPERTY(BlueprintReadWrite)
		bool Thunder=false;
	UPROPERTY(BlueprintReadWrite)
		bool Laser=false;
	UPROPERTY(BlueprintReadWrite)
		bool Spin=false;
	UPROPERTY(BlueprintReadWrite)
		bool BossHeal=false;
	UPROPERTY(BlueprintReadWrite)
		bool part2=false;
};
