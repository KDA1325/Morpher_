// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_MoveToPlayer.h"
#include "MyAI.h"
#include "AIController.h"
#include "EntityPreset.h"
#include "CharacterAllInterface.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "DrawDebugHelpers.h"

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

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (BlackboardComp == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    APawn* playerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (playerPawn == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    AEntityPreset* Entity = Cast<AEntityPreset>(ControllingPawn);
    if (Entity && Entity->bIsCastingSkill)
    {
        UE_LOG(LogTemp, Warning, TEXT("Skipping movement because skill is in progress"));
        return EBTNodeResult::Failed;
    }

	//if(Entity && Entity->bIsCastingSkill)
	//{
	//	if(playerPawn && OwnerComp.GetAIOwner())
	//	{
	//		OwnerComp.GetAIOwner()->SetFocus(playerPawn);
	//	}
	//	return EBTNodeResult::Failed;
	//}

    float DistanceToPlayer = BlackboardComp->GetValueAsFloat(BBKEY_DISTANCE);
    float MinDistance = BlackboardComp->GetValueAsFloat(BBKEY_FOLLOWMINDISTANCE);
    float MonsterCapsuleRadius = BlackboardComp->GetValueAsFloat(BBKEY_MONSTERRADIUS);
    float PlayerCapsuleRadius = BlackboardComp->GetValueAsFloat(BBKEY_PLAYERRADIUS);

    // 실제 최소 거리 계산
    float CalculatedMinDistance = MinDistance + PlayerCapsuleRadius + MonsterCapsuleRadius;

    if (DistanceToPlayer > CalculatedMinDistance)
    {
        FVector PlayerLocation = BlackboardComp->GetValueAsVector(BBKEY_PLAYERLOCATION);
        OwnerComp.GetAIOwner()->SetFocus(playerPawn);
        OwnerComp.GetAIOwner()->MoveToActor(playerPawn, CalculatedMinDistance, false);
        return EBTNodeResult::Succeeded;
    }
    else
    {
		OwnerComp.GetAIOwner()->SetFocus(playerPawn);
        OwnerComp.GetAIOwner()->StopMovement();
    }

    return EBTNodeResult::Succeeded;
}