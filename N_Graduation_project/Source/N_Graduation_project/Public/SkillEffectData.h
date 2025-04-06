// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"  // FTableRowBase 사용을 위한 헤더 추가 
#include "Engine/GameInstance.h"
#include "SkillEffectEnum.h"
#include "SkillEffectData.generated.h"


USTRUCT(BlueprintType)
struct FSkillEffectData : public FTableRowBase
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillEffect")
    FString SkillNameID = "";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillEffect")
    FString EffectID = "";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillEffect")
    EnumEffectType EffectType = EnumEffectType::Damage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillEffect")
    float EffectValue01 = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillEffect")
    float EffectValue02 = 0;
};

