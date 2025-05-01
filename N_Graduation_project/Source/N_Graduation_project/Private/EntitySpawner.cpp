// Fill out your copyright notice in the Description page of Project Settings.


#include "EntitySpawner.h"

// Sets default values
AEntitySpawner::AEntitySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEntitySpawner::BeginPlay()
{
	Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("EntitySpawner BeginPlay - Group ID: %s"), *EntityGroupID);
}

// Called every frame
void AEntitySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


AEntityPreset* AEntitySpawner::SpawnEntityCharacter()
{
    FVector SpawnLocation = GetActorLocation();
    FRotator SpawnRotation = GetActorRotation();


    // 그룹 ID를 이용해 엔티티 데이터를 먼저 불러오기
    if (!UABGameSingleton::Get().GetEntityDataByGroupID(EntityGroupID, EntityData))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find entity data for Group ID: %s"), *EntityGroupID);
        return nullptr;
    }

    // EntityGroupID에 해당하는 EntityPreset 찾기 
    TSubclassOf<AEntityPreset>* MatchBPClass = EntityPresetPaths.Find(EntityGroupID);
    if (MatchBPClass == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("EntityPreset Class is not set"));
        return nullptr;
    }

    // EntityPreset 스폰 
    AEntityPreset* SpawnedEntity = GetWorld()->SpawnActor<AEntityPreset>(*MatchBPClass, SpawnLocation, SpawnRotation);
    if (SpawnedEntity)
    {
        // 스폰 직후 데이터 세팅
        SpawnedEntity->InitializeEntity(EntityData);
        UE_LOG(LogTemp, Warning, TEXT("Entity spawned with Group ID: %s"), *EntityGroupID);

        return SpawnedEntity;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn entity character"));
    }

    return nullptr;
}

