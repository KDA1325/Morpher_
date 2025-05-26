// Fill out your copyright notice in the Description page of Project Settings.

#include "AutoLevelStreamingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LevelStreaming.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"


// Sets default values for this component's properties
UAutoLevelStreamingComponent::UAutoLevelStreamingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAutoLevelStreamingComponent::BeginPlay()
{
	//Super::BeginPlay();

	//// 이 Actor의 BeginOverlap에 이벤트 연결
	//if(AActor* Owner = GetOwner())
	//{
	//	Owner->OnActorBeginOverlap.AddDynamic(this,&UAutoLevelStreamingComponent::OnTriggerOverlap);
	//}
}


// Called every frame
void UAutoLevelStreamingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
}

TArray<FName> UAutoLevelStreamingComponent::GetLoadedStreamingLevelNames(UObject * WorldContextObject)
{
	TArray<FName> LoadedLevelNames;

	if(UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject))
	{
		for(ULevelStreaming* StreamingLevel : World->GetStreamingLevels())
		{
			if(StreamingLevel && StreamingLevel->IsLevelLoaded())
			{
				FString PackageName = StreamingLevel->GetWorldAssetPackageName();
				LoadedLevelNames.Add(FName(*FPackageName::GetShortFName(PackageName).ToString()));
			}
		}
	}
	return LoadedLevelNames;
}
//
//void UAutoLevelStreamingComponent::OnTriggerOverlap(AActor* OverlappedActor,AActor* OtherActor)
//{
//	//// 플레이어만 작동하도록 설정
//	//if(!OtherActor->IsA(ACharacter::StaticClass())) return;
//
//	////FString LevelRootPath = "/Game/Asset/SyntyAsset/MainMapAsset/PNB_Meadow_Forest/Maps/Forest_Level/";
//
//	//// 현재 레벨에 존재하는 스트리밍 레벨들 가져오기
//	//UWorld* World = GetWorld();
//	//if(!World) return;
//
//	//// 현재 로드된 레벨 이름 저장
//	//TSet<FName> CurrentLoadedLevels;
//
//	//for(ULevelStreaming* StreamingLevel : World->GetStreamingLevels())
//	//{
//	//	if(StreamingLevel && StreamingLevel->IsLevelLoaded())
//	//	{
//	//		FName LevelName = FName(*FPackageName::GetShortFName(StreamingLevel->GetWorldAssetPackageName()).ToString());
//	//		
//	//		CurrentLoadedLevels.Add(LevelName);
//	//	}
//	//}
//
//	//// ConnectedLevels 로드
//	//for(FName LevelName : ConnectedLevels)
//	//{
//	//	//FString FullPath = LevelRootPath + LevelName.ToString();
//	//	//FName FullLevelName(*FullPath);
//
//	//	/*UGameplayStatics::LoadStreamLevel(this,FullLevelName,true,false,FLatentActionInfo());
//	//	LoadedLevels.Add(LevelName);
//
//	//	UE_LOG(LogTemp,Error,TEXT("Load Levels: %s"),*FullLevelName.ToString());*/
//
//	//	if(!CurrentLoadedLevels.Contains(LevelName))
//	//	{
//	//		UGameplayStatics::LoadStreamLevel(this,LevelName,true,false,FLatentActionInfo());
//	//		//LoadedLevels.Add(LevelName);
//
//	//		UE_LOG(LogTemp,Error,TEXT("Load Levels: %s"),*LevelName.ToString());
//	//	}
//	//}
//
//	//for(FName Loaded : CurrentLoadedLevels)
//	//{
//	//	if(!ConnectedLevels.Contains(Loaded))
//	//	{
//	//		//FString FullPath = LevelRootPath + Loaded.ToString();
//	//		//FName FullLevelName(*FullPath);
//
//	//		UGameplayStatics::UnloadStreamLevel(this,Loaded,FLatentActionInfo(),false);
//
//	//		UE_LOG(LogTemp,Error,TEXT("UnLoad Levels: %s"),*Loaded.ToString());
//	//	}
//	//}
//
//	////// 현재 로드되어 있는 레벨 중 ConnectedLevels에 없는 레벨 언로드 
//	////TArray<FName> ToUnload;
//	////for(FName Loaded : LoadedLevels)
//	////{
//	////	FString FullPath = LevelRootPath + Loaded.ToString();
//	////	FName FullLevelName(*FullPath);
//
//	////	if(!ConnectedLevels.Contains(Loaded))
//	////	{
//	////		UGameplayStatics::UnloadStreamLevel(this,FullLevelName,FLatentActionInfo(),false);
//	////		ToUnload.Add(Loaded);
//
//	////		UE_LOG(LogTemp,Error,TEXT("UnLoad Levels: %s"),*FullLevelName.ToString());
//	////	}
//	////}
//
//	///*for(FName LevelName : ToUnload)
//	//{
//	//	LoadedLevels.Remove(LevelName);
//	//}*/
//}