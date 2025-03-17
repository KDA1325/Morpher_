// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameFramework/Actor.h"
#include "MyAIController.generated.h"

UCLASS()
class N_GRADUATION_PROJECT_API AMyAIController : public AAIController
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AMyAIController();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

private:
    UPROPERTY(EditAnywhere)
    class UBehaviorTree* MonsterBehaviorTree;
};