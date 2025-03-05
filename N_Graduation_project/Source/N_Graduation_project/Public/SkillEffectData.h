// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"  // FTableRowBase 사용을 위한 헤더 추가 
#include "Engine/GameInstance.h"
#include "SkillEffectData.generated.h"

USTRUCT(BlueprintType)
struct FSkillEffectData : public FTableRowBase
{
    GENERATED_BODY()

public:
    FSkillEffectData()
        : SkillNameID(TEXT("SkillNameID")), EffectID(TEXT("EffectID")), EffectType(TEXT("EffectType")), EffectValue01(0), EffectValue02(0) {
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    FString SkillNameID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    FString EffectID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    FString EffectType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    float EffectValue01;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    float EffectValue02;
};


