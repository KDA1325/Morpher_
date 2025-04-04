// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SkillEffectEnum.generated.h"

// 스킬 효과 타입 Enum
UENUM(BlueprintType)
enum class EnumEffectType : uint8
{
    Damage        UMETA(DisplayName = "Damage"),
    KnockBack      UMETA(DisplayName = "KnockBack"),
    Destroy     UMETA(DisplayName = "Destroy"),
    Fire     UMETA(DisplayName = "Fire"),
    Freezing     UMETA(DisplayName = "Freezing"),
    Stun     UMETA(DisplayName = "Stun"),
    Guard     UMETA(DisplayName = "Guard"),
    AOEDamage        UMETA(DisplayName = "AOEDamage"),
    // 필요한 값 추가...
};