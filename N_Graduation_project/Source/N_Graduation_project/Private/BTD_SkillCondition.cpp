// Fill out your copyright notice in the Description page of Project Settings.


#include "BTD_SkillCondition.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MyAI.h"

UBTD_SkillCondition::UBTD_SkillCondition()
{
    NodeName = TEXT("Skill Condition");

    ObserverAborts = EBTFlowAbortMode::Both;
}

//bool UBTD_SkillCondition::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
//{
//    if (UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent())
//    {
//        // Blackboard로부터 값 읽어오기
//        float Distance = BlackboardComp->GetValueAsFloat(BBKEY_DISTANCE);
//        float A_SkillRange = BlackboardComp->GetValueAsFloat(BBKEY_ASKILLRANGE);
//        float B_SkillRange = BlackboardComp->GetValueAsFloat(BBKEY_BSKILLRANGE);
//        //bool bA_SkillAvailable = BlackboardComp->GetValueAsBool(BBKEY_BASKILLAVAILABLE);
//        //bool bB_SkillAvailable = BlackboardComp->GetValueAsBool(BBKEY_BBSKILLAVAILABLE);
//
//        // 스킬 쿨타임이 구현 되어있지 않은 관계로 Range 비교 결과로만 스킬 시전 
//        // 조건 계산:
//        // A스킬 조건: Distance <= ARange AND AAvailable true
//        // B스킬 조건: Distance <= BRange AND BAvailable true
//        /*bool bASkillCondition = (Distance <= A_SkillRange) && bA_SkillAvailable;
//        bool bBSkillCondition = (Distance <= B_SkillRange) && bB_SkillAvailable;*/
//        
//        bool bASkillCondition = (Distance <= A_SkillRange);
//        bool bBSkillCondition = (Distance <= B_SkillRange);
//
//        UE_LOG(LogTemp, Warning, TEXT("bASkillCondition: %s"), bASkillCondition ? TEXT("True") : TEXT("False"));
//        UE_LOG(LogTemp, Warning, TEXT("bBSkillCondition: %s"), bBSkillCondition ? TEXT("True") : TEXT("False"));
//        return (bASkillCondition || bBSkillCondition);
//    }
//
//    return false;
//}