// Fill out your copyright notice in the Description page of Project Settings.


#include "MySaveGame.h"

UMySaveGame::UMySaveGame()
{
	Level = 1;
	LevelName = TEXT("Forest_Tutorial");
}
// 이렇게 하고
// 트리거 볼륨으로 플레이어가 들어오면 
// Level++;
// 다음 맵 이름을 LevelName에 넣기