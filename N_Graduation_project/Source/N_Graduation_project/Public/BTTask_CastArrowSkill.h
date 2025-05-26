// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_CastArrowSkill.generated.h"

/**
 * 
 */
UCLASS()
class N_GRADUATION_PROJECT_API UBTTask_CastArrowSkill : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_CastArrowSkill();

protected:
	// BTTaskNode 인터페이스 재정의 (실행)
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp,uint8* NodeMemory) override;

	// 태스크의 진행 중에 사용할 캐시된 BehaviorTreeComponent 포인터
	UPROPERTY()
		UBehaviorTreeComponent* CachedOwnerComp;
	
};
