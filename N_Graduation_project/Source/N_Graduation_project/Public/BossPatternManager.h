// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "BossPatternManager.generated.h"

UENUM(BlueprintType)
enum class EPatternType: uint8
{
	Thunder,
	Laser,

};
USTRUCT(BlueprintType)
struct FPatternData
{
	GENERATED_BODY()
		FPatternData()
		: PatternType(EPatternType::Thunder) // 기본값 설정 (필요하면 변경)
		,Delay(0.f)
		,Thundercount(0.f) // ← 여기서 반드시 초기화
	{}
	UPROPERTY(EditAnywhere)
		EPatternType PatternType;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		float Delay;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		float Thundercount;;
};


UCLASS()
class N_GRADUATION_PROJECT_API ABossPatternManager: public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABossPatternManager();

	void Thunder();
	void SpawnAndAttachLasers();

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		float Delay;//Thunder delay
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		int ThunderCount;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		float ThunderDamage;
	UFUNCTION(BlueprintCallable)

	void ApplyThunderDamage();

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AActor> LaserBPClass;

	UPROPERTY(EditDefaultsOnly)
		TArray<FName> LaserSocketNames;

protected:
	virtual void BeginPlay() override;
	void SpawnThunder();

	UPROPERTY(EditAnywhere,Category = "Spawn")
		TSubclassOf<AActor> ThunderBPClass;

	FTimerHandle ThunderTimerHandle;
	int32 ThunderSpawnCount = 0;

	UPROPERTY(VisibleAnywhere,Category="Component")
		USphereComponent* SphereComponent;

	UPROPERTY(EditAnywhere)
		AActor* BossActor;

	UPROPERTY(EditAnywhere)
		float BossHeadHeightOffset = 200.f;  // 머리 위 오프셋
	//UFUNCTION(BlueprintCallable)
	//	void PlayThunderAnimation();

};