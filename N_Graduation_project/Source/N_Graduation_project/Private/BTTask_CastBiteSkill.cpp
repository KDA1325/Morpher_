#include "BTTask_CastBiteSkill.h"
#include "EntityPreset.h"
#include "EntitySkillComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Animation/AnimInstance.h"

UBTTask_CastBiteSkill::UBTTask_CastBiteSkill()
{
	NodeName = TEXT("Cast Bite Skill");
	CachedOwnerComp = nullptr;
}

EBTNodeResult::Type UBTTask_CastBiteSkill::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 캐시 저장
	CachedOwnerComp = &OwnerComp;

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
		return EBTNodeResult::Failed;

	OwnerComp.GetAIOwner()->StopMovement();

	AEntityPreset* Entity = Cast<AEntityPreset>(ControllingPawn);
	if (!Entity)
		return EBTNodeResult::Failed;

	// 이미 스킬 실행 중이면 중복 실행하지 않음
	if (Entity->bIsCastingSkill)
		return EBTNodeResult::Failed;

	//Entity->bIsCastingSkill = true; // 스킬 진행 중 상태 설정
	Entity->EntitySkillComponent->ExecuteSkill("Skill_Bite");


	//// Bite 스킬 실행
	//if (Entity->NormalSkillMontage)
	//{
	//	if (UAnimInstance* AnimInst = Entity->GetMesh()->GetAnimInstance())
	//	{
	//		// 스킬 실행 시작 플래그 설정 (업데이트 서비스에서 조건을 갱신하지 않도록)
	//		Entity->bIsCastingSkill = true;
	//		// 몽타주 재생
	//		AnimInst->Montage_Play(Entity->NormalSkillMontage);
	//		UE_LOG(LogTemp, Warning, TEXT("BTTask_CastBiteSkill: Bite Montage played"));

	//		// 몽타주 종료 델리게이트 바인딩
	//		FOnMontageEnded EndDelegate;
	//		EndDelegate.BindUObject(Entity, &AEntityPreset::OnSkillMontageEnded);
	//		AnimInst->Montage_SetEndDelegate(EndDelegate, Entity->NormalSkillMontage);

	//		// Bite 스킬의 HitBox도 활성화
	//		Entity->ShowNormalHitBox();

	//		return EBTNodeResult::InProgress;
	//	}
	//}
	return EBTNodeResult::InProgress;
}

// 몽타주 종료 시 호출되며 스킬 실행 상태를 해제
void UBTTask_CastBiteSkill::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (CachedOwnerComp)
	{
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
		CachedOwnerComp = nullptr;
	}
}
