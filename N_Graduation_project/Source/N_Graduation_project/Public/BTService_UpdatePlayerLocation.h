// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdatePlayerLocation.generated.h"

/**
 * 
 */
UCLASS()
class N_GRADUATION_PROJECT_API UBTService_UpdatePlayerLocation : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_UpdatePlayerLocation();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
