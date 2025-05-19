// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class N_GRADUATION_PROJECT_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:	
		UPROPERTY(BlueprintReadOnly)
		bool GetKey = false;
};
