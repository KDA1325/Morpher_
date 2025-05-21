// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MySaveGame.generated.h"

USTRUCT(BlueprintType)
struct FRoomSaveData
{
	GENERATED_BODY()

		UPROPERTY()
		FName RoomID;

	UPROPERTY()
		bool bIsCleared;
};

UCLASS()
class N_GRADUATION_PROJECT_API UMySaveGame: public USaveGame
{
	GENERATED_BODY()
public:
	UMySaveGame();

	UPROPERTY()
		FString RoomName;
		UPROPERTY()
		FString PlayerPreset;

	UPROPERTY()
		TArray<FName> DiscoveredEntities;

	UPROPERTY()
		float FullHP;
	UPROPERTY()
		float SaveFinalAngle;

	UPROPERTY()
		FVector RespawnLocation;

	
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
