#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_CastBiteSkill.generated.h"

UCLASS()
class N_GRADUATION_PROJECT_API UBTTask_CastBiteSkill : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_CastBiteSkill();

protected:
	// BTTaskNode 인터페이스 재정의 (실행)
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// 몽타주 종료 델리게이트 콜백 (Bite 스킬용)
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// 태스크의 진행 중에 사용할 캐시된 BehaviorTreeComponent 포인터
	UPROPERTY()
	UBehaviorTreeComponent* CachedOwnerComp;
};
