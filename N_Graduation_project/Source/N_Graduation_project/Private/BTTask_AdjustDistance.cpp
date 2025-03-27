// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_AdjustDistance.h"
#include "MyAI.h"
#include "AIController.h"
#include "CharacterAllInterface.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_AdjustDistance::UBTTask_AdjustDistance()
{
	NodeName = TEXT("Adjust Distance");
}

EBTNodeResult::Type UBTTask_AdjustDistance::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

    float DistanceToPlayer = BlackboardComp->GetValueAsFloat(BBKEY_DISTANCE);
    float MonsterCapsuleRadius = BlackboardComp->GetValueAsFloat(BBKEY_MONSTERRADIUS);
    float PlayerCapsuleRadius = BlackboardComp->GetValueAsFloat(BBKEY_PLAYERRADIUS);

    float MinDistance = 300.0f;

    // 실제 최소 거리 계산
    float CalculatedMinDistance = MinDistance + PlayerCapsuleRadius + MonsterCapsuleRadius;
    
    FVector MonsterLocation = ControllingPawn->GetActorLocation();
    FVector PlayerLocation = BlackboardComp->GetValueAsVector(BBKEY_PLAYERLOCATION);
 
    if (DistanceToPlayer == CalculatedMinDistance)
    {
        OwnerComp.GetAIOwner()->StopMovement();
    }
    else if (DistanceToPlayer > CalculatedMinDistance)
    {
        OwnerComp.GetAIOwner()->SetFocus(playerPawn);
        OwnerComp.GetAIOwner()->MoveToActor(playerPawn, CalculatedMinDistance, false);
        return EBTNodeResult::Succeeded;
    }
    else if (DistanceToPlayer < CalculatedMinDistance)
    {
        OwnerComp.GetAIOwner()->SetFocus(playerPawn);
        // (몬스터 - 플레이어) 방향 → 플레이어의 반대 방향
        FVector FleeDirection = (MonsterLocation - PlayerLocation).GetSafeNormal();

        // 도망 목표 지점: 플레이어 위치에서 FleeDirection으로 CalcMinDistance만큼 떨어진 곳
        FVector FleeLocation = PlayerLocation + FleeDirection * CalculatedMinDistance;

        // AcceptanceRadius는 너무 크게 잡지 않도록 적절히 설정
        OwnerComp.GetAIOwner()->MoveToLocation(FleeLocation, /*AcceptanceRadius=*/5.0f, false);

        //// 플레이어로부터 벡터를 계산하여 반대 방향(도망 방향)으로 정규화
        //FVector RunAwayDirection = (MonsterLocation - PlayerLocation).GetSafeNormal();

        //// 도망 목표 위치: 플레이어 위치에서 RunAwayDirection으로 MinDistance만큼 떨어진 지점
        //FVector AdjustLocation = PlayerLocation + RunAwayDirection * MinDistance;

        //OwnerComp.GetAIOwner()->MoveToLocation(AdjustLocation);
        return EBTNodeResult::Succeeded;
    }

	return EBTNodeResult::Type();
}
