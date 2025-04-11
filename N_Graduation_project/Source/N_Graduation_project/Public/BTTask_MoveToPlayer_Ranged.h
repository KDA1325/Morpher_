// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MoveToPlayer_Ranged.generated.h"

/**
 * 
 */
UCLASS()
class N_GRADUATION_PROJECT_API UBTTask_MoveToPlayer_Ranged : public UBTTaskNode
{
	GENERATED_BODY()
	
	UBTTask_MoveToPlayer_Ranged();

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	//EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
private:
	/*FAIRequestID MoveRequestID;
	UPROPERTY()
	UBehaviorTreeComponent* CachedOwnerComp;
	UFUNCTION()*/
	//void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);
};
