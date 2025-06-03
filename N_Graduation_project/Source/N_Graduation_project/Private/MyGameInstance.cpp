#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "N_Graduation_project/N_Graduation_projectCharacter.h"
#include "MyPlayerStatComponent.h"
#include "Engine/LatentActionManager.h"

void UMyGameInstance::Init()
{
	Super::Init();
}

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

void UMyGameInstance::SaveToSaveData(UMySaveGame* SaveData)
{
	if(!SaveData) return;

	SaveData->Open_Boar = WildBoar_Ok;
	SaveData->Open_MonKey = Inpermon_OK;
	SaveData->Open_Prezard = SkeletonWarrior_OK;
	SaveData->Open_Skel1 = StoneGolem_OK;
	SaveData->Open_Skel2 = SkeletonArcher_OK;
	SaveData->Open_Golem = Freezard_OK;
	SaveData->FullHP = PlayerFullHP;
	SaveData->RoomName = SaveRoomName;
	SaveData->SaveFinalAngle = PlayerFinalAngle;
	SaveData->SavePlayerLocation = SaveLocation;
	UE_LOG(LogTemp,Warning,TEXT("SaveGame SaveRoomName: %s"),*SaveData->RoomName.ToString());
}

void UMyGameInstance::SaveGame()
{
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

		SaveRoomName = SaveData->RoomName;
		SaveLocation = SaveData->SavePlayerLocation;
		PlayerFullHP = SaveData->FullHP;

		bWaitingLevelLoad = true; // Tick 시작
	}
}

void UMyGameInstance::OnLevelLoaded()
{
	UE_LOG(LogTemp,Warning,TEXT("OnLevelLoaded called!"));

	ULevelStreaming* Level = UGameplayStatics::GetStreamingLevel(this,SaveRoomName);
	if(Level)
	{
		Level->SetShouldBeVisible(true);
	} else
	{
		UE_LOG(LogTemp,Warning,TEXT("Streaming Level NOT Found for %s"),*SaveRoomName.ToString());
	}

	AN_Graduation_projectCharacter* Player = Cast<AN_Graduation_projectCharacter>(UGameplayStatics::GetPlayerCharacter(this,0));
	if(!Player) return;

	FVector LoadLoc = SaveLocation + FVector(0,0,100);
	UE_LOG(LogTemp,Warning,TEXT("Teleporting player to: %s"),*LoadLoc.ToString());
	Player->SetActorLocation(LoadLoc);
	// 방어 코드 추가
	if(Player->IsValidLowLevelFast())
	{
		Player->LoadChangePreset();
	}
	if(UMyPlayerStatComponent* Stat = Player->FindComponentByClass<UMyPlayerStatComponent>())
	{
		Stat->SetHP(PlayerFullHP);
	}

	Player->ChangePreset("PlayerCharacter");
	bWaitingLevelLoad = false; // Tick 중단
}

void UMyGameInstance::Tick(float DeltaTime)
{
	ULevelStreaming* Level = UGameplayStatics::GetStreamingLevel(this,SaveRoomName);
	if(Level && Level->IsLevelLoaded())
	{
		// Player가 유효할 때만 OnLevelLoaded 실행
		ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this,0);
		if(Player && Player->IsValidLowLevel())
		{
			OnLevelLoaded();
		}
	}
}
