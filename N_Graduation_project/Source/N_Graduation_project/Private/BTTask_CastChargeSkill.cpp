#include "BTTask_CastChargeSkill.h"
#include "EntityPreset.h"
#include "EntitySkillComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_CastChargeSkill::UBTTask_CastChargeSkill()
{
	NodeName = TEXT("Cast Charge Skill");
	CachedOwnerComp = nullptr;
}

EBTNodeResult::Type UBTTask_CastChargeSkill::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	CachedOwnerComp = &OwnerComp;

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
		return EBTNodeResult::Failed;

	OwnerComp.GetAIOwner()->StopMovement();

	AEntityPreset* Entity = Cast<AEntityPreset>(ControllingPawn);
	if (!Entity)
		return EBTNodeResult::Failed;

	if (Entity->bIsCastingSkill)
		return EBTNodeResult::Failed;

	// Charge 스킬 실행 : Charge 스킬은 AEntityPreset의 PerformSkill_Charge()에서 전체 Charge 로직을 처리합니다.
	//Entity->bIsCastingSkill = true; // 스킬 진행 중 상태 설정
	Entity->EntitySkillComponent->ExecuteSkill("Skill_Charge");

	// Entity->PerformSkill_Charge();

	// Charge 스킬이 내부적으로 몽타주 종료 델리게이트를 통해 bIsCastingSkill을 false로 클리어하면,
	// 이 태스크는 InProgress 상태에서 OnMontageEnded에서 완료 처리합니다.
	return EBTNodeResult::InProgress;
}

void UBTTask_CastChargeSkill::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (CachedOwnerComp)
	{
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
		CachedOwnerComp = nullptr;
	}
}
