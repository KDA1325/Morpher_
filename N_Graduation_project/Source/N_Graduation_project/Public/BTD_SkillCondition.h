// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_SkillCondition.generated.h"

/**
 * 
 */
UCLASS()
class N_GRADUATION_PROJECT_API UBTD_SkillCondition : public UBTDecorator
{
	GENERATED_BODY()
public:
    UBTD_SkillCondition();

    UPROPERTY(EditAnywhere, Category = "Decorator")
    TEnumAsByte<EBTFlowAbortMode::Type> ObserverAborts;

protected:
    // 이 함수에서 조건을 평가
    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
};
