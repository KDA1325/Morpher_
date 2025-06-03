#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MySaveGame.h"
#include "Tickable.h"
#include "MyGameInstance.generated.h"

/**
 *
 */
UCLASS()
class N_GRADUATION_PROJECT_API UMyGameInstance: public UGameInstance,public FTickableGameObject
{
	GENERATED_BODY()
public:
	//UMyGameInstance();

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

	//UPROPERTY(BlueprintReadWrite)
	//	bool bHasSeenSplash = false;

	UFUNCTION()
		void OnLevelLoaded();

	bool Player_Ok = true;
	bool WildBoar_Ok = false;
	bool Inpermon_OK = true;
	bool SkeletonWarrior_OK = false;
	bool StoneGolem_OK = false;
	bool SkeletonArcher_OK = false;
	bool Freezard_OK = false;

	float PlayerFullHP = 150;
	float PlayerFinalAngle = 102.f;

	// 게임 시작 시 자동 로드
	void Init() override;

	// 세이브 / 로드 함수
	UFUNCTION(BlueprintCallable)
		void SaveGame();
	UFUNCTION(BlueprintCallable)
		void LoadGame();

	// 내부 사용 함수
	void InitFromSaveData(UMySaveGame* SaveData);
	void SaveToSaveData(UMySaveGame* SaveData);

	// Tick 관련 함수
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override {
		RETURN_QUICK_DECLARE_CYCLE_STAT(UMyGameInstance,STATGROUP_Tickables);
	}
	virtual bool IsTickable() const override {
		return bWaitingLevelLoad;
	}

	// 보스 관련 플래그
	UPROPERTY(BlueprintReadWrite)
		bool Thunder = false;
	UPROPERTY(BlueprintReadWrite)
		bool Laser = false;
	UPROPERTY(BlueprintReadWrite)
		bool Spin = false;
	UPROPERTY(BlueprintReadWrite)
		bool BossHeal = false;
	UPROPERTY(BlueprintReadWrite)
		bool part2 = false;

private:
	bool bWaitingLevelLoad = false;
};