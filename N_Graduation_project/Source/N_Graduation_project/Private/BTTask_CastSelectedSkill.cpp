#include "BTTask_CastSelectedSkill.h"
#include "MyAI.h"
#include "EntityPreset.h"
#include "EntitySkillComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_CastSelectedSkill::UBTTask_CastSelectedSkill()
{
	NodeName = TEXT("Cast Selected Skill");
	CachedOwnerComp = nullptr;
}

EBTNodeResult::Type UBTTask_CastSelectedSkill::ExecuteTask(UBehaviorTreeComponent& OwnerComp,uint8* NodeMemory)
{
	CachedOwnerComp = &OwnerComp;

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if(!ControllingPawn)
		return EBTNodeResult::Failed;

	OwnerComp.GetAIOwner()->StopMovement();

	AEntityPreset* Entity = Cast<AEntityPreset>(ControllingPawn);
	if(!Entity || !Entity->EntitySkillComponent)
		return EBTNodeResult::Failed;

	if(Entity->bIsCastingSkill)
		return EBTNodeResult::Failed;

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if(BlackboardComp == nullptr)
	{
		return EBTNodeResult::Failed;
	}
		
	FString SkillID = BlackboardComp->GetValueAsString(BBKEY_CASTSKILLID);

	if(SkillID.IsEmpty())
		return EBTNodeResult::Failed;

	Entity->EntitySkillComponent->ExecuteSkill(SkillID);

	return EBTNodeResult::InProgress;
}
