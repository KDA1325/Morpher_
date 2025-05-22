// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BossPatternManager.h"

#include "BossCharacter.generated.h"

class BossPatternManager;

UCLASS()
class N_GRADUATION_PROJECT_API ABossCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABossCharacter();

	UPROPERTY(BlueprintReadWrite)
		bool SkillStart;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
	ABossPatternManager* BossPatternManager;

	void Pattern1();
};
