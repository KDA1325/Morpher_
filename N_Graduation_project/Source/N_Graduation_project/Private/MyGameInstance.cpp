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
	SaveData->NextRoomName = SaveNextRoomName;
	SaveData->SaveFinalAngle = PlayerFinalAngle;
	SaveData->SavePlayerLocation = SaveLocation;
	UE_LOG(LogTemp,Warning,TEXT("SaveGame SaveRoomName: %s"),*SaveData->RoomName.ToString());
}

void UMyGameInstance::SaveGame()
{
	UMySaveGame* SaveData = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
	if(!SaveData) return;
	if(isSave==false){ 
		UE_LOG(LogTemp,Warning,TEXT("OnLevelLoaded isSave!"));

	isSave=true;}
	SaveToSaveData(SaveData);
	UGameplayStatics::SaveGameToSlot(SaveData,TEXT("MySaveSlot"),0);
}

void UMyGameInstance::LoadGame()
{
	AN_Graduation_projectCharacter* Player = Cast<AN_Graduation_projectCharacter>(UGameplayStatics::GetPlayerCharacter(this,0));
	if(!Player) return;

	const FString SlotName = TEXT("MySaveSlot");
	const int32 UserIndex = 0;

	if(UGameplayStatics::DoesSaveGameExist(SlotName,UserIndex))
	{
		USaveGame* LoadedGame = UGameplayStatics::LoadGameFromSlot(SlotName,0);
		UMySaveGame* SaveData = Cast<UMySaveGame>(LoadedGame);
		if(!SaveData) return;

		InitFromSaveData(SaveData);
		SaveRoomName = SaveData->RoomName;
		SaveLocation = SaveData->SavePlayerLocation;
		PlayerFullHP = SaveData->FullHP;

		// 레벨 스트리밍으로 저장된 방 로드
		FLatentActionInfo LatentInfo;
		LatentInfo.CallbackTarget = this;
		LatentInfo.ExecutionFunction = FName("OnLevelLoaded");
		LatentInfo.Linkage = 0;
		LatentInfo.UUID = 1;
bWaitingLevelLoad = true; // Tick 시작
		UGameplayStatics::LoadStreamLevel(this,SaveData->RoomName,true,false,LatentInfo);
		Player->LoadChangePreset();

		// 레벨 스트리밍으로 저장된 방 로드
		//FLatentActionInfo LatentInfo1;
		//LatentInfo1.CallbackTarget = this;
		//LatentInfo1.ExecutionFunction = FName("OnNextLevelLoaded");
		//LatentInfo1.Linkage = 0;
		//LatentInfo1.UUID = 1;
		//bWaitingLevelLoad = true; // Tick 시작
		//UGameplayStatics::LoadStreamLevel(this,SaveData->RoomName,true,false,LatentInfo1);
		////Player->LoadChangePreset();
	}
}

void UMyGameInstance::OnLevelLoaded()
{
	if(!bWaitingLevelLoad) return;
	bWaitingLevelLoad = false;

	UE_LOG(LogTemp,Warning,TEXT("OnLevelLoaded called!"));

	ULevelStreaming* Level = UGameplayStatics::GetStreamingLevel(this,SaveRoomName);
	if(!Level)
	{
		UE_LOG(LogTemp,Error,TEXT("Streaming Level NOT Found: %s"),*SaveRoomName.ToString());
		return;
	}

	Level->SetShouldBeVisible(true);

	auto* Player = Cast<AN_Graduation_projectCharacter>(UGameplayStatics::GetPlayerCharacter(this,0));
	if(!Player) return;

	Player->SetActorLocation(SaveLocation + FVector(0,0,100));

	if(auto* Stat = Player->FindComponentByClass<UMyPlayerStatComponent>())
	{
		Stat->SetHP(PlayerFullHP);
	}

	Player->ChangePreset("PlayerCharacter");

	UE_LOG(LogTemp,Warning,TEXT("Teleported player to %s in level %s"),*SaveLocation.ToString(),*SaveRoomName.ToString());
}
void UMyGameInstance::OnNextLevelLoaded()
{
	if(!bWaitingLevelLoad) return;
	bWaitingLevelLoad = false;

	UE_LOG(LogTemp,Warning,TEXT("OnLevelLoaded called!"));

	ULevelStreaming* Level = UGameplayStatics::GetStreamingLevel(this,SaveNextRoomName);
	if(!Level)
	{
		UE_LOG(LogTemp,Error,TEXT("Streaming Level NOT Found: %s"),*SaveNextRoomName.ToString());
		return;
	}

	Level->SetShouldBeVisible(true);

	UE_LOG(LogTemp,Warning,TEXT("Teleported player to %s in level %s"),*SaveLocation.ToString(),*SaveRoomName.ToString());
}

void UMyGameInstance::Tick(float DeltaTime)
{
	//ULevelStreaming* Level = UGameplayStatics::GetStreamingLevel(this,SaveRoomName);
	//if(Level && Level->IsLevelLoaded())
	//{
	//	// Player가 유효할 때만 OnLevelLoaded 실행
	//	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this,0);
	//	if(Player && Player->IsValidLowLevel())
	//	{
	//		OnLevelLoaded();
	//	}
	//}
}
