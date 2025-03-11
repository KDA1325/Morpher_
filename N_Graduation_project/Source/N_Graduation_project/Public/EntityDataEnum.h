// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "EntityDataEnum.generated.h"

// 공격 타입 Enum
UENUM(BlueprintType)
enum class EnumAttackType : uint8
{
    Melee        UMETA(DisplayName = "Melee"),
    Ranged      UMETA(DisplayName = "Ranged"),
    // 필요한 값 추가...
};