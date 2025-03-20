// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_SkillCheck.generated.h"

/**
 * 
 */
UCLASS()
class N_GRADUATION_PROJECT_API UBTService_SkillCheck : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_SkillCheck();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};

