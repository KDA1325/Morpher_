// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_CastArmSwingSkill.h"
#include "EntityPreset.h"
#include "EntitySkillComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_CastArmSwingSkill::UBTTask_CastArmSwingSkill()
{
	NodeName = TEXT("Cast Arm Swing Skill");
	CachedOwnerComp = nullptr;
}

EBTNodeResult::Type UBTTask_CastArmSwingSkill::ExecuteTask(UBehaviorTreeComponent& OwnerComp,uint8* NodeMemory)
{
	CachedOwnerComp = &OwnerComp;

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if(!ControllingPawn)
		return EBTNodeResult::Failed;

	OwnerComp.GetAIOwner()->StopMovement();

	AEntityPreset* Entity = Cast<AEntityPreset>(ControllingPawn);
	if(!Entity)
		return EBTNodeResult::Failed;

	if(Entity->bIsCastingSkill)
		return EBTNodeResult::Failed;

	// 스킬 실행
	Entity->EntitySkillComponent->ExecuteSkill("Skill_ArmSwing");
	UE_LOG(LogTemp,Error,TEXT("Skill_ArmSwing"));

	// 스킬이 내부적으로 몽타주 종료 델리게이트를 통해 bIsCastingSkill을 false로 클리어하면,
	// 이 태스크는 InProgress 상태에서 OnMontageEnded에서 완료 처리
	return EBTNodeResult::InProgress;
}