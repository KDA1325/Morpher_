#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_CastSelectedSkill.generated.h"

UCLASS()
class N_GRADUATION_PROJECT_API UBTTask_CastSelectedSkill: public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_CastSelectedSkill();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp,uint8* NodeMemory) override;

protected:
	UPROPERTY()
		UBehaviorTreeComponent* CachedOwnerComp;
};
