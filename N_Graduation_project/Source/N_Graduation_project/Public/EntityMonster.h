// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
#include "EngineMinimal.h" // UStaticMeshComponent를 사용하기 위해 변경
#include "GameFramework/Character.h"
#include "Components/WidgetComponent.h"
#include "ABEntityData.h" // Entity Data 구조체
#include "ABGameSingleton.h"
#include "EntityMonster.generated.h"

UCLASS()
class N_GRADUATION_PROJECT_API AEntityMonster : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEntityMonster();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	int32 maxHp;
	int32 moveSpeed;
	FString normalSkill;
	FString specialSkill;
	FString presetReference;


	int32 currentHp;
	int32 currentSpeed;
	FString currentPreset;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//// 블루프린트에서 설정할 GroupID
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	//FString EntityGroupID;

	//// ABEntityData 구조체 사용
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data")
	//FABEntityData EntityData;

	//// 설정한 GroupID를 키 값으로 가져온 데이터를 적용하는 함수
	//UFUNCTION(BlueprintCallable, Category = "Data")
	//void UpdateEntityData();

	//void UpdateEntityWidget();

	//// EntityPreset 블루프린트 클래스를 참조하는 변수
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	//TSubclassOf<AActor> EntityPresetClass;

	//UFUNCTION(BlueprintCallable, Category = "Spawn")
	//void SpawnEntityPreset();

	//// MaxHP를 설정하는 함수
	//void SetMaxHp(int32 MaxHp);
	//// MoveSpeed를 설정하는 함수
	//void SetMoveSpeed(int32 MoveSpeed);
	//// Preset을 설정하는 함수
	//void SetPreset(FString PresetReference);

	//// 메시 변경을 위해 
	//UPROPERTY(VisibleAnywhere)
	//USkeletalMeshComponent* m_pMeshCom;

	//// EntityPreset 블루프린트에서 사용할 위젯 클래스
	////UPROPERTY()
	////class UEntityWidget* EntityWidget; // EntityWidget은 UUserWidget을 상속한 클래스

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//bool isSpawned;
};
