#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "N_Graduation_project/N_Graduation_projectCharacter.h"
#include "MyPlayerStatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LatentActionManager.h"
void UMyGameInstance::Init()
{
	Super::Init();
	//	UE_LOG(LogTemp,Warning,TEXT("LoadGame SaveLocation: X=%f, Y=%f, Z=%f"),SaveLocation.X,SaveLocation.Y,SaveLocation.Z);

}

void UMyGameInstance::InitFromSaveData(UMySaveGame* SaveData)
{
	if(!SaveData) return;

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
	SaveData->SaveFinalAngle=PlayerFinalAngle;
	SaveData->SavePlayerLocation = SaveLocation;
//	UE_LOG(LogTemp,Warning,TEXT("LoadGame SaveToSaveData SaveLocation: X=%f, Y=%f, Z=%f"),SaveData->SavePlayerLocation.X,SaveData->SavePlayerLocation.Y,SaveData->SavePlayerLocation.Z);
	UE_LOG(LogTemp,Warning,TEXT("SaveGame SaveRoomName: %s"),*SaveData->RoomName.ToString());

}

void UMyGameInstance::SaveGame()
{
//	UE_LOG(LogTemp,Warning,TEXT("SaveGame실행."));

	UMySaveGame* SaveData = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
	if(!SaveData) return;

	SaveToSaveData(SaveData);

	UGameplayStatics::SaveGameToSlot(SaveData,TEXT("MySaveSlot"),0);
}

void UMyGameInstance::LoadGame()
{
	const FString SlotName = TEXT("MySaveSlot");
	const int32 UserIndex = 0;

	if(UGameplayStatics::DoesSaveGameExist(SlotName,UserIndex))
	{
		USaveGame* LoadedGame = UGameplayStatics::LoadGameFromSlot(SlotName,UserIndex);
		UMySaveGame* SaveData = Cast<UMySaveGame>(LoadedGame);
		if(!SaveData) return;

		InitFromSaveData(SaveData);

		// 레벨 스트리밍으로 저장된 방 로드
		FLatentActionInfo LatentInfo;
		LatentInfo.CallbackTarget = this;
		LatentInfo.ExecutionFunction = FName("OnLevelLoaded");
		LatentInfo.Linkage = 0;
		LatentInfo.UUID = 1;
		
		UGameplayStatics::LoadStreamLevel(this,SaveData->RoomName,true,false,LatentInfo);

		// 플레이어 위치 세팅은 OnLevelLoaded에서 처리
		SaveRoomName = SaveData->RoomName;
		SaveLocation = SaveData->SavePlayerLocation;
		PlayerFullHP = SaveData->FullHP;
	}
}

// 레벨 스트리밍 완료 콜백
void UMyGameInstance::OnLevelLoaded()
{
	UE_LOG(LogTemp,Warning,TEXT("OnLevelLoaded called!"));

	ULevelStreaming* Level = UGameplayStatics::GetStreamingLevel(this,SaveRoomName);
	if(Level)
	{
		//UE_LOG(LogTemp,Warning,TEXT("Streaming Level Found - IsLoaded: %d, IsVisible: %d"),
		//	Level->IsLevelLoaded(),Level->GetShouldBeVisible());

		Level->SetShouldBeVisible(true); // 강제 표시
	} else
	{
		UE_LOG(LogTemp,Warning,TEXT("Streaming Level NOT Found for %s"),*SaveRoomName.ToString());
	}

	AN_Graduation_projectCharacter* Player = Cast<AN_Graduation_projectCharacter>(UGameplayStatics::GetPlayerCharacter(this,0));
	if(!Player) return;

	FVector LoadLoc = SaveLocation + FVector(0,0,300);
	UE_LOG(LogTemp,Warning,TEXT("Teleporting player to: %s"),*LoadLoc.ToString());
	Player->SetActorLocation(LoadLoc);

	if(UMyPlayerStatComponent* Stat = Player->FindComponentByClass<UMyPlayerStatComponent>())
	{
		Stat->SetHP(PlayerFullHP);
	}

	Player->ChangePreset("PlayerCharacter");
	Player->isDead = false;
}
