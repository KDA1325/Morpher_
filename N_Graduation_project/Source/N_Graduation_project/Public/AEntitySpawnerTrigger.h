// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tester.h"
#include "GameFramework/Actor.h"
#include "AEntitySpawnerTrigger.generated.h"

UCLASS()
class N_GRADUATION_PROJECT_API AAEntitySpawnerTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAEntitySpawnerTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
