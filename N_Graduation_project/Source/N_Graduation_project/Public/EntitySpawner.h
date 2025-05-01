// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ABEntityData.h" // Entity Data 구조체
#include "ABGameSingleton.h"
//#include "EntityCharacter.h"
#include "EntityPreset.h"
#include "GameFramework/Actor.h"
#include "EntitySpawner.generated.h"

UCLASS()
class N_GRADUATION_PROJECT_API AEntitySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEntitySpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly)
	//AEntityCharacter* EntityCharacter;

	// EntityCharacterClass 클래스를 참조하는 변수 
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity")
	//TSubclassOf<AEntityCharacter> EntityCharacterClass;

	// ABEntityData 구조체 사용
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data")
	FABEntityData EntityData;

	// 에디터에서 설정할 GroupID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FString EntityGroupID;
	
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	AEntityPreset* SpawnEntityCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TMap<FString, TSubclassOf<AEntityPreset>> EntityPresetPaths;
};
