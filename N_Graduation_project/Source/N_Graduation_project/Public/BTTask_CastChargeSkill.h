#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_CastChargeSkill.generated.h"

UCLASS()
class N_GRADUATION_PROJECT_API UBTTask_CastChargeSkill : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_CastChargeSkill();

protected:
	// BTTaskNode 인터페이스 재정의 (실행)
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// 몽타주 종료 델리게이트 콜백 (Charge 스킬용)
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// 캐시
	UPROPERTY()
	UBehaviorTreeComponent* CachedOwnerComp;
};
