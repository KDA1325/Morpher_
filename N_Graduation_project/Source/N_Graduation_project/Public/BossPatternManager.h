// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
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

		UPROPERTY(EditAnywhere)
		EPatternType PatternType;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		float Delay ;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		float Thundercount;
};


UCLASS()
class N_GRADUATION_PROJECT_API ABossPatternManager : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABossPatternManager();
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		float Delay;//Thunder delay
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int ThunderCount;

	void Thunder();
protected:
	virtual void BeginPlay() override;
	void SpawnThunder();

	UPROPERTY(EditAnywhere,Category = "Spawn")
		TSubclassOf<AActor> ThunderBPClass;
	FTimerHandle ThunderTimerHandle;
	int32 ThunderSpawnCount = 0;
};