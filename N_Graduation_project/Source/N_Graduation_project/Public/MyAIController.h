// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "MyAIController.generated.h"

// 순환 참조를 방지하기 위한 전방 선언 
class AEntityPreset;

UCLASS()
class N_GRADUATION_PROJECT_API AMyAIController : public AAIController
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AMyAIController();

    void RunAI();
    void StopAI();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    virtual void OnPossess(APawn* InPawn) override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    void DefineSkillPriority(AEntityPreset* PossessedCharacter);

private:
    UPROPERTY()
    TObjectPtr<class UBlackboardData> BBMonster;

    UPROPERTY()
    TObjectPtr<class UBehaviorTree> BTMonster;

    UPROPERTY()
    UBlackboardComponent* BlackboardComp;
};