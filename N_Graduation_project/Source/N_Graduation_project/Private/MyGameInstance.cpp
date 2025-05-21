#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "N_Graduation_project/N_Graduation_projectCharacter.h"
#include "MyPlayerStatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LatentActionManager.h"
void UMyGameInstance::Init()
{
	Super::Init();
	UE_LOG(LogTemp,Error,TEXT("LoadGamePreset,Init CurrentPlayerCharacter: %s"),*CurrentPlayerCharacter);
	//	UE_LOG(LogTemp,Warning,TEXT("LoadGame SaveLocation: X=%f, Y=%f, Z=%f"),SaveLocation.X,SaveLocation.Y,SaveLocation.Z);

}

void UMyGameInstance::InitFromSaveData(UMySaveGame* SaveData)
{
	if(!SaveData) return;
	CurrentPlayerCharacter = SaveData->PlayerPreset;

	WildBoar_Ok         = SaveData->Open_Boar;
	Inpermon_OK         = SaveData->Open_MonKey;
	SkeletonWarrior_OK  = SaveData->Open_Prezard;
	StoneGolem_OK       = SaveData->Open_Skel1;
	SkeletonArcher_OK   = SaveData->Open_Skel2;
	Freezard_OK         = SaveData->Open_Golem;

}

void UMyGameInstance::SaveToSaveData(UMySaveGame* SaveData)
{
	if(!SaveData) return;

	SaveData->Open_Boar  = WildBoar_Ok;
	SaveData->Open_MonKey = Inpermon_OK;
	SaveData->Open_Prezard = SkeletonWarrior_OK;
	SaveData->Open_Skel1 = StoneGolem_OK;
	SaveData->Open_Skel2 = SkeletonArcher_OK;
	SaveData->Open_Golem = Freezard_OK;
	SaveData->FullHP = PlayerFullHP;
	SaveData->RoomName = SaveRoomName;
	SaveData->PlayerPreset = CurrentPlayerCharacter;
	SaveData->SaveFinalAngle=PlayerFinalAngle;
	SaveData->SavePlayerLocation = SaveLocation;
	UE_LOG(LogTemp,Warning,TEXT("LoadGame 완료 후 CurrentPlayerCharacter: %s %s"),*CurrentPlayerCharacter,*SaveData->PlayerPreset);
	UE_LOG(LogTemp,Warning,TEXT("LoadGame SaveToSaveData SaveLocation: X=%f, Y=%f, Z=%f"),SaveData->SavePlayerLocation.X,SaveData->SavePlayerLocation.Y,SaveData->SavePlayerLocation.Z);

}

void UMyGameInstance::SaveGame()
{
	UE_LOG(LogTemp,Warning,TEXT("SaveGame실행."));

	UMySaveGame* SaveData = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
	if(!SaveData) return;

	SaveToSaveData(SaveData);

	UGameplayStatics::SaveGameToSlot(SaveData,TEXT("MySaveSlot"),0);
}

void UMyGameInstance::LoadGame()
{
	UE_LOG(LogTemp,Warning,TEXT("LoadGame실행."));
	const FString SlotName = TEXT("MySaveSlot");
	const int32 UserIndex = 0;
	if(UGameplayStatics::DoesSaveGameExist(SlotName,UserIndex))
	{
		UE_LOG(LogTemp,Warning,TEXT("LoadGame DoesSaveGameExist실행."));
		USaveGame* LoadedGame = UGameplayStatics::LoadGameFromSlot(SlotName,UserIndex);
		UMySaveGame* SaveData = Cast<UMySaveGame>(LoadedGame);

		if(!SaveData)
		{
			UE_LOG(LogTemp,Warning,TEXT("LoadGame: SaveGame 파일이 현재 SaveGame 클래스와 호환되지 않음(삭제하겟단뜻)"));

			// 경로 수동 구성 및 삭제
			FString SaveDir = FPaths::ProjectSavedDir() + TEXT("SaveGames/");
			FString FullPath = SaveDir + SlotName + TEXT(".sav");
			IFileManager::Get().Delete(*FullPath);

			return;
		}

		InitFromSaveData(SaveData);
		UE_LOG(LogTemp,Error,TEXT("LoadGamePreset, CurrentPlayerCharacter: %s"),*CurrentPlayerCharacter);
		AN_Graduation_projectCharacter* Player = Cast<AN_Graduation_projectCharacter>(UGameplayStatics::GetPlayerCharacter(this,0));
		if(Player)
		{
			UE_LOG(LogTemp,Warning,TEXT("LoadGame Player실행."));

			//UE_LOG(LogTemp,Error,TEXT("LoadGamePreset, CurrentPlayerCharacter: %s"),*CurrentPlayerCharacter);

			if(UMyPlayerStatComponent* Stat = Player->FindComponentByClass<UMyPlayerStatComponent>())
			{
				Stat->SetHP(PlayerFullHP);
			}
			if(SaveData)
			{
				UE_LOG(LogTemp,Warning,TEXT("LoadGame_ Boar Opened: %s"),SaveData->Open_Boar ? TEXT("Yes") : TEXT("No"));
				UE_LOG(LogTemp,Warning,TEXT("LoadGame_ Current Preset: %s"),*SaveData->PlayerPreset);
				UE_LOG(LogTemp,Warning,TEXT("LoadGame_ PlayerFullHP: %f"),SaveData->FullHP);
				Player->LoadPreset(SaveData->PlayerPreset);
				UE_LOG(LogTemp,Warning,TEXT("LoadGame_ SaveFinalAngle: %f"),SaveData->SaveFinalAngle);
				SaveLocation = SaveData->SavePlayerLocation;
				SaveLocation.Z += 100.0f; // Z축 위로 이동!
				Player->SetActorLocation(SaveLocation);
				UE_LOG(LogTemp,Warning,TEXT("LoadGame LoadGame SaveLocation: X=%f, Y=%f, Z=%f"),SaveLocation.X,SaveLocation.Y,SaveLocation.Z);

			}
			FLatentActionInfo LatentInfo;
			LatentInfo.CallbackTarget = this;
			LatentInfo.ExecutionFunction = FName("OnLevelLoaded");
			LatentInfo.Linkage = 0;
			LatentInfo.UUID = __LINE__; // 고유 ID

			UGameplayStatics::LoadStreamLevel(this,FName(SaveData->RoomName),true,false,LatentInfo);
		}
	}
}

