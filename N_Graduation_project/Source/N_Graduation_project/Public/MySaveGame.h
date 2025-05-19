// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MySaveGame.generated.h"

/**
 *
 */
UCLASS()
class N_GRADUATION_PROJECT_API UMySaveGame: public USaveGame
{
	GENERATED_BODY()
public:
	UMySaveGame();

	UPROPERTY()
		int32 Level; // 몇번째 맵인지 체크하려고(=방 클리어 여부)
	UPROPERTY()
		FString LevelName; // 맵 이름 저장(=같은 맵을 열기위해서)

	UPROPERTY()
		bool Puzzle; // 퍼즐 해결 여부

	UPROPERTY()
		bool Open_Boar;
	UPROPERTY()
		bool Open_MonKey;
	UPROPERTY()
		bool Open_Prezard;
	UPROPERTY()
		bool Open_Skel1;
	UPROPERTY()
		bool Open_Skel2;
	UPROPERTY()
		bool Open_Golem;
	
	// 세이브할 것
		//도감 진척도
		//방 클리어 여부
		// 퍼즐 해결 여부

};
