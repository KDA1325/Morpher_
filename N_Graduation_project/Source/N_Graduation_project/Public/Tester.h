// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// #include "CoreMinimal.h"
#include "EngineMinimal.h" // UStaticMeshComponent를 사용하기 위해 변경
#include "GameFramework/Actor.h"
#include "ABEntityData.h" // Entity Data 구조체
#include "ABGameSingleton.h"
#include "Tester.generated.h"

UCLASS()
class N_GRADUATION_PROJECT_API ATester : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATester();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 블루프린트에서 설정할 GroupID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FString EntityGroupID;

	// ABEntityData 구조체 사용
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data")
	FABEntityData EntityData;

	// 설정한 GroupID를 키 값으로 가져온 데이터를 적용하는 함수
	UFUNCTION(BlueprintCallable, Category = "Data")
	void UpdateEntityData();
	
	// EntityPreset 블루프린트 클래스를 참조하는 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TSubclassOf<AActor> EntityPresetClass;

	void SpawnEntityPreset();

	int32 maxHp;
	int32 moveSpeed;
	FString normalSkill;
	FString specialSkill;
	FString presetReference;


	int32 currentHp;
	int32 currentSpeed;
	FString currentPreset;
	
	// MaxHP를 설정하는 함수
	void SetMaxHp(int32 MaxHp);
	// MoveSpeed를 설정하는 함수
	void SetMoveSpeed(int32 MoveSpeed);
	// Preset을 설정하는 함수
	void SetPreset(FString PresetReference);

	// 메시 변경 테스트를 위해 
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* m_pMeshCom;

	// EntityPreset 블루프린트에서 사용할 위젯 클래스
	UPROPERTY()
	class UEntityWidget* EntityWidget; // EntityWidget은 UUserWidget을 상속한 클래스

};
