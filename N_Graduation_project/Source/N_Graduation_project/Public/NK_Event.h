// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NK_Event.generated.h"

UCLASS()
class N_GRADUATION_PROJECT_API ANK_Event : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANK_Event();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
