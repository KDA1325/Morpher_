// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "BossProjectile.h"


#include "BossPatternManager.generated.h"

UENUM(BlueprintType)
enum class EPatternType: uint8
{
	Thunder,
	Laser,
	Spin,
	Heal,
	Meteor,
};
USTRUCT(BlueprintType)
struct FPatternData
{
	GENERATED_BODY()
	//	FPatternData()
	//	: PatternType(EPatternType::Thunder) // 기본값 설정 (필요하면 변경)
	//	,Delay(0.f)
	//	,Thundercount(0.f) // ← 여기서 반드시 초기화
	//{}
	UPROPERTY(EditAnywhere)
		EPatternType PatternType;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		float Delay;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		float Thundercount=5;
};


UCLASS()
class N_GRADUATION_PROJECT_API ABossPatternManager: public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABossPatternManager();
protected:
	virtual void BeginPlay() override;

public:
	//전기기둥
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

	UPROPERTY(EditAnywhere,Category = "Spawn")
		TSubclassOf<AActor> ThunderBPClass;
	bool thunderOnce=false;
protected:
	//레이저
	void SpawnThunder();

	FTimerHandle ThunderTimerHandle;
	int32 ThunderSpawnCount = 0;

	UPROPERTY(VisibleAnywhere,Category="Component")
		USphereComponent* SphereComponent;

	UPROPERTY(EditAnywhere)
		AActor* BossActor;

	UPROPERTY(EditAnywhere)
		float BossHeadHeightOffset = 200.f;  // 머리 위 오프셋
public:
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AActor> LaserBPClass;

	UPROPERTY(EditDefaultsOnly)
		TArray<FName> LaserSocketNames;

public:
	// 회전 투사체
	UPROPERTY(EditDefaultsOnly)
		TArray<FName> Spinning1SocketNames;
	UPROPERTY(EditDefaultsOnly)
		TArray<FName> Spinning2SocketNames;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AActor> SpinningBPClass;

	UFUNCTION(BlueprintCallable)
		void SpinningBarrage();
	UFUNCTION(BlueprintCallable)
		void SpinningBarrage2();

	void StartSpinningBarrageSequence(int num);
	void SpinningBarrageTick();
	void StartSpinningBarrageSequence2(int num);
	void SpinningBarrageTick2();
	
private:
	int ProjectileSpeed=300;
	float ApplyDamageAmount=10.f;
	int SpinningBarrageCount=0;
	FTimerHandle SpinningBarrageTimerHandle;
	FTimerHandle SpinningBarrageTimerHandle2;
	// 기존 includes 및 클래스 정의 안에 아래 함수 추가

public:
	//회복 수정체
	UPROPERTY(EditDefaultsOnly)
		TArray<FName> CrystalSocketNames;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AActor> BlackCryBPClass;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AActor> BlueCryBPClass;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AActor> RedCryBPClass;

	void HealCrystal();

	//메테오
	UFUNCTION(BlueprintCallable)
		void ApplyMeteorDamage();
	// Meteor 관련 변수
	FTimerHandle MeteorTimerHandle;
	int32 MeteorSpawnCount = 0;
	UPROPERTY(BlueprintReadWrite)
	int32 MeteorTotalCount = 50;
	UPROPERTY(BlueprintReadWrite)
	int32 MeteorsPerSecond = 1;
	void Meteor();
	void SpawnMeteorBatch();
	TSubclassOf<AActor> MeteorBPClass;
	FVector GetRandomMeteorLocation(); // 랜덤 생성 함수

	//ㅠㅔ이즈
void StartPhase1();
void StartPhase2();

private:
	int NextPatternIndex = 0;
};