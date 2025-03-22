// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FleeFromPlayer.generated.h"

/**
 * 
 */
UCLASS()
class N_GRADUATION_PROJECT_API UBTTask_FleeFromPlayer : public UBTTaskNode
{
	GENERATED_BODY()
	
	UBTTask_FleeFromPlayer();

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

};
