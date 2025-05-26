// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_CastArmSwingSkill.generated.h"

/**
 * 
 */
UCLASS()
class N_GRADUATION_PROJECT_API UBTTask_CastArmSwingSkill : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_CastArmSwingSkill();

protected:
	// BTTaskNode 인터페이스 재정의 (실행)
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp,uint8* NodeMemory) override;

	//// 몽타주 종료 델리게이트 콜백
	//UFUNCTION()
	//	void OnMontageEnded(UAnimMontage* Montage,bool bInterrupted);

	// 캐시
	UPROPERTY()
		UBehaviorTreeComponent* CachedOwnerComp;
	
};
