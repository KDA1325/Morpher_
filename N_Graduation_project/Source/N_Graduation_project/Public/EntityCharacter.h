// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
#include "EngineMinimal.h" // USkeletalMeshComponent를 사용하기 위해 변경
#include "ABEntityData.h" // Entity Data 구조체
#include "ABGameSingleton.h"
#include "GameFramework/Character.h"
#include "EntityCharacter.generated.h"

UCLASS()
class N_GRADUATION_PROJECT_API AEntityCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEntityCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	FString m_entityGroupID;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// EntitySpawer의 EnttityGroupID를 받아오는 함수
	UFUNCTION(BlueprintCallable, Category = "Data")
	void GetEntityGroupID(FString EntityGroupID);

	// 받아온 GroupID를 키 값으로 가져온 데이터를 적용하는 함수
	UFUNCTION(BlueprintCallable, Category = "Data")
	void UpdateEntityData();

	// MaxHP를 설정하는 함수
	void SetMaxHp(int32 MaxHp);
	// MoveSpeed를 설정하는 함수
	void SetMoveSpeed(int32 MoveSpeed);
	// Preset을 설정하는 함수
	void SetPreset(FString PresetReference);

	// 메시 변경을 위한 변수 선언 
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* m_pMeshCom;
};
