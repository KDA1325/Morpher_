// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include "MySaveGame.h"

void UMyGameInstance::InitFromSaveData(UMySaveGame* SaveData)
{
	if(!SaveData) return;

	WildBoar_Ok = SaveData->Open_Boar;
	Inpermon_OK = SaveData->Open_MonKey;
	SkeletonWarrior_OK = SaveData->Open_Prezard;
	StoneGolem_OK = SaveData->Open_Skel1;
	SkeletonArcher_OK = SaveData->Open_Skel2;
	Freezard_OK = SaveData->Open_Golem;
}