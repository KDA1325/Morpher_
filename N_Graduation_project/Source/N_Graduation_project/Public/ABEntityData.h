#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"  // FTableRowBase 사용을 위한 헤더 추가 
#include "Engine/GameInstance.h"
#include "EntityDataEnum.h"
#include "ABEntityData.generated.h"

USTRUCT(BlueprintType)
struct FABEntityData : public FTableRowBase
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    FString EntityGroupID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    FString EntityName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    int32 HP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    int32 MoveSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    FString NormalSkill;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    FString SpecialSkill;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    int32 TransManaCost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    FString PresetReference;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    int32 CapsuleRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    EnumAttackType AttackType;
};

