// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"  // FTableRowBase 사용을 위한 헤더 추가 
#include "Engine/GameInstance.h"
#include "SkillData.generated.h"

USTRUCT(BlueprintType)
struct FSkillData : public FTableRowBase
{
    GENERATED_BODY()

public:
    FSkillData()
        : SkillID(TEXT("ID")), SkillName(TEXT("SkillName")), SkillType(TEXT("SkillType")), SkillRange(0), SkillDuration(0), SkillCoolTime(0), SkillTypeShape(TEXT("SkillTypeShape")), SkillTypeSizeX(0), SkillTypeSizeY(0), ProjectileSpeed(0) {
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    FString SkillID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    FString SkillName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    FString SkillType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    int32 SkillRange;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    float SkillDuration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    float SkillCoolTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    FString SkillTypeShape;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    int32 SkillTypeSizeX;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    int32 SkillTypeSizeY;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    int32 ProjectileSpeed;
};

