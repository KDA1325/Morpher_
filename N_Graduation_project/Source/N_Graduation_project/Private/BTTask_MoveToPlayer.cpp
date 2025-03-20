// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveToPlayer.h"
#include "MyAI.h"
#include "AIController.h"
#include "CharacterAllInterface.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_MoveToPlayer::UBTTask_MoveToPlayer()
{
	NodeName = TEXT("Move To Player");
}

EBTNodeResult::Type UBTTask_MoveToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();

	if (nullptr == ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}

	ICharacterAllInterface* AIPawn = Cast<ICharacterAllInterface>(ControllingPawn);
	if (nullptr == AIPawn)
	{
		return EBTNodeResult::Failed;
	}

	// 플레이어 가져오기
	ACharacter* PlayerCharacter = Cast<ACharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKEY_TARGET));
	if (nullptr == PlayerCharacter)
	{
		return EBTNodeResult::Failed;
	}

	// ControllingPawn과 플레이어 사이의 거리 저장 
	float DistanceToPlayer = FVector::Dist(ControllingPawn->GetActorLocation(), PlayerCharacter->GetActorLocation());
	
	// 최소 거리 
	float MinDistance = 30.0f;

	// ControllingPawn과 플레이어 사이의 거리는 최소 거리 이상으로 가까워지지 않는다
	if (DistanceToPlayer <= MinDistance)
	{
		return EBTNodeResult::Succeeded;
	}

	// AI Owner 액터가 PlayerCharacter를 목표지점으로, MinDistance만큼 거리까지 이동, Overlap되면 stop
	OwnerComp.GetAIOwner()->MoveToActor(PlayerCharacter, MinDistance, true);

	return EBTNodeResult::Succeeded;
}
