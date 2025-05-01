// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_CastSkill.h"
#include "MyAI.h"
#include "EntityPreset.h"
#include "EntitySkillComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_CastSkill::UBTTask_CastSkill()
{
    NodeName = TEXT("Cast Skill");
}

// 비동기 대기 처리 
EBTNodeResult::Type UBTTask_CastSkill::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

    // 캐시(임시 저장)
    CachedOwnerComp = &OwnerComp;

    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
    if (ControllingPawn == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    OwnerComp.GetAIOwner()->StopMovement();

    AEntityPreset* PossessedEntity = Cast<AEntityPreset>(ControllingPawn);
    if (PossessedEntity == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (BlackboardComp == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    FString SeletedSkillID = BlackboardComp->GetValueAsString(BBKEY_SELECTEDSKILLID);
    if (SeletedSkillID.IsEmpty())
    {
        return EBTNodeResult::Failed;
    }

    if (PossessedEntity->EntitySkillComponent)
    {
        PossessedEntity->EntitySkillComponent->ExecuteSkill(SeletedSkillID);

        return EBTNodeResult::InProgress;
    }

    ////// 노멀 스킬 시전 함수 호출 (몽타주 재생, 애님 노티파이 등)
    ////PossessedEntity->PerformNormalSkill();
    ////return EBTNodeResult::Succeeded;

    //// 애니메이션 인스턴스를 가져옴
    //if (UAnimInstance* AnimInst = PossessedEntity->GetMesh()->GetAnimInstance())
    //{
    //    // 몽타주 종료 델리게이트 바인딩
    //    FOnMontageEnded EndDelegate;
    //    EndDelegate.BindUObject(this, &UBTTask_CastSkill::OnMontageEnded);
    //    AnimInst->Montage_SetEndDelegate(EndDelegate, PossessedEntity->NormalSkillMontage);

    //    // 노멀 스킬 몽타주 재생 (이 함수 내부에서 애니메이션 재생과 관련된 추가 로직을 수행할 수 있습니다.)
    //    AnimInst->Montage_Play(PossessedEntity->NormalSkillMontage);

    //    // 몽타주 종료까지 "진행 중" 상태로 반환
    //    return EBTNodeResult::InProgress;
    //}

    return EBTNodeResult::Failed;
}

//// 몽타주 종료 델리게이트 콜백 함수
//void UBTTask_CastSkill::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
//{
//    // 애니메이션 몽타주가 종료되었으면 FinishLatentTask()를 호출하여 태스크를 완료 처리
//    if (CachedOwnerComp)
//    {
//        FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
//        CachedOwnerComp = nullptr; // 캐시 변수 초기화
//    }
//}
