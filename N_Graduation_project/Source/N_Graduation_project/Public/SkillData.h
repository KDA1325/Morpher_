// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"  // FTableRowBase 사용을 위한 헤더 추가 
#include "Engine/GameInstance.h"
#include "SkillEnum.h"
#include "SkillData.generated.h"

USTRUCT(BlueprintType)
struct FSkillData : public FTableRowBase
{
    GENERATED_BODY()

public:

    // 스킬 이름 ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    FString SkillNameID;

    // 스킬 이름
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    FString SkillName;

    // 스킬 타입 (Enum)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    EnumSkillType SkillType;

    // 스킬 사정거리
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    float SkillRange;

    // 스킬 지속 시간
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    float SkillDuration;

    // 스킬 쿨타임
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    float SkillCoolTime;

    // 스킬 타입 모양 (Enum)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    EnumSkillTypeShape SkillTypeShape;

    // 모양의 X 크기
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    float SkillTypeSizeX;

    // 모양의 Y 크기
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    float SkillTypeSizeY;

    // 투사체 속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    float ProjectileSpeed;
};