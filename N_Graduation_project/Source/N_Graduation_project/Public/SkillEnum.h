// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SkillEnum.generated.h"

// 스킬 타입 Enum
UENUM(BlueprintType)
enum class EnumSkillType : uint8
{
    Buff        UMETA(DisplayName = "Buff"),
    HitBox      UMETA(DisplayName = "HitBox"),
    Projectile     UMETA(DisplayName = "Projectile"),
    // 필요한 값 추가...
};

// 스킬 모양 Enum
UENUM(BlueprintType)
enum class EnumSkillTypeShape : uint8
{
    None        UMETA(DisplayName = "None"),
    Box      UMETA(DisplayName = "Box"),
    Sphere        UMETA(DisplayName = "Sphere"),
    // 필요한 값 추가...
};