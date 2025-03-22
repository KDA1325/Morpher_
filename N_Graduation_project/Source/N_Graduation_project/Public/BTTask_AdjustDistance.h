// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AdjustDistance.generated.h"

/**
 * 
 */
UCLASS()
class N_GRADUATION_PROJECT_API UBTTask_AdjustDistance : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_AdjustDistance();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
