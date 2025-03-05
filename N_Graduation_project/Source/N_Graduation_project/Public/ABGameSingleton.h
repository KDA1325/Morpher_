// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ABEntityData.h" //Entity데이터 가져오려고(Entity Data 구조체)
#include "SkillData.h" //SkillData데이터 가져오려고
#include "SkillEffectData.h" //SkillEffectData데이터 가져오려고
#include "ABGameSingleton.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogABGameSingleton, Error, All);

UCLASS()
class N_GRADUATION_PROJECT_API UABGameSingleton : public UObject
{
	GENERATED_BODY()

public:
	UABGameSingleton();
	static UABGameSingleton& Get();
	//~ABGameSingleton();

	// Get함수, 값이 유효하면 전자, 아니면 그냥 기본 생성자를 만들어서 넘겨주도록 선언
	FORCEINLINE FABEntityData GetCharacterStat(int32 ID) const
	{
		return EntityDataTable.IsValidIndex(ID - 1) ? EntityDataTable[ID - 1] : FABEntityData();
	}

	// EntityGroupID를 기준으로 데이터를 가져오는 함수
	UFUNCTION(BlueprintCallable, Category = "Data")
	bool GetEntityDataByGroupID(const FString& GroupID, FABEntityData& OutEntityData) const;

	UPROPERTY()
	// 총 몇개의 Entity?
	int32 HowManyEntity;
	int32 HowManySkill;
	int32 HowManySkillEffect;

private:
	TArray<FABEntityData> EntityDataTable; // 테이블들을 내부적으로 보관해서 필요한 게임 객체들에게 제공하기 위한 Talbe Array
	TMap<FString, FABEntityData> EntityDataMap; // EntityGroupID를 키 값으로 데이터를 저장하는 Map
	
	TArray<FSkillData> SkillDataTable; // 테이블들을 내부적으로 보관해서 필요한 게임 객체들에게 제공하기 위한 Talbe Array
	//TMap<FString, FSkillData> FSkillDataMap;

	TArray<FSkillEffectData> SkillEffectDataTable; // 테이블들을 내부적으로 보관해서 필요한 게임 객체들에게 제공하기 위한 Talbe Array
	//TMap<FString, FSkillEffectData> FSkillEffectDataMap;
};
