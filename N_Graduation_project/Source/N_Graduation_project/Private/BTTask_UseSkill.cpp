// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_UseSkill.h"
#include "MyAI.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CharacterAllInterface.h"

UBTTask_UseSkill::UBTTask_UseSkill()
{
	NodeName = TEXT("Use Skill");
}

EBTNodeResult::Type UBTTask_UseSkill::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();

	if (nullptr == ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}

	ICharacterAllInterface* AIPawn = Cast<ICharacterAllInterface>(ControllingPawn);
	if (nullptr == AIPawn)
	{
		return EBTNodeResult::Failed;
	}

	// 스킬 사용 
	// ControllingPawn의 스킬 데이터를 받아와서 
	/*if (AIPawn->USeSkill(skilldata))
	
		return EBTNodeResult::Succeeded;
	}*/

	return EBTNodeResult::Failed;
}


