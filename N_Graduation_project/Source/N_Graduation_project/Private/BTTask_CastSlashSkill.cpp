// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_CastSlashSkill.h"
#include "EntityPreset.h"
#include "EntitySkillComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Animation/AnimInstance.h"

UBTTask_CastSlashSkill::UBTTask_CastSlashSkill()
{
	NodeName = TEXT("Cast Skeleton Slash Skill");
	CachedOwnerComp = nullptr;
}

EBTNodeResult::Type UBTTask_CastSlashSkill::ExecuteTask(UBehaviorTreeComponent& OwnerComp,uint8* NodeMemory)
{
	// 캐시 저장
	CachedOwnerComp = &OwnerComp;

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if(!ControllingPawn)
		return EBTNodeResult::Failed;

	OwnerComp.GetAIOwner()->StopMovement();

	AEntityPreset* Entity = Cast<AEntityPreset>(ControllingPawn);
	if(!Entity)
		return EBTNodeResult::Failed;

	// 이미 스킬 실행 중이면 중복 실행하지 않음
	if(Entity->bIsCastingSkill)
		return EBTNodeResult::Failed;

	//Entity->bIsCastingSkill = true; // 스킬 진행 중 상태 설정
	Entity->EntitySkillComponent->ExecuteSkill("Skill_SkeletonSlash");

	return EBTNodeResult::InProgress;
}