// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_CastSkill.h"
#include "EntityPreset.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_CastSkill::UBTTask_CastSkill()
{
    NodeName = TEXT("Cast Skill");
}

EBTNodeResult::Type UBTTask_CastSkill::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AICont = OwnerComp.GetAIOwner();
    if (AICont)
    {
        AEntityPreset* MyEntity = Cast<AEntityPreset>(AICont->GetPawn());
        if (MyEntity)
        {
            // 노멀 스킬 시전 함수 호출 (몽타주 재생, 애님 노티파이 등)
            MyEntity->PerformNormalSkill();
            return EBTNodeResult::Succeeded;
        }
    }
    return EBTNodeResult::Failed;
}
