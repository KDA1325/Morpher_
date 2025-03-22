// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_MoveToPlayer_Ranged.h"
#include "MyAI.h"
#include "AIController.h"
#include "CharacterAllInterface.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_MoveToPlayer_Ranged::UBTTask_MoveToPlayer_Ranged()
{
    NodeName = TEXT("Move To Player_Ranged");
}

EBTNodeResult::Type UBTTask_MoveToPlayer_Ranged::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
    if (ControllingPawn == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (BlackboardComp == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (PlayerPawn == nullptr)
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

    // 거리가 CalculatedMinDistance보다 크면 플레이어 쪽으로 접근
    if (DistanceToPlayer > CalculatedMinDistance)
    {
        OwnerComp.GetAIOwner()->MoveToActor(PlayerPawn, CalculatedMinDistance, false);
    }
    else
    {
        OwnerComp.GetAIOwner()->StopMovement();
    }

    return EBTNodeResult::Succeeded;
}


//// Fill out your copyright notice in the Description page of Project Settings.
//
//
//#include "BTTask_MoveToPlayer_Ranged.h"
//#include "MyAI.h"
//#include "AIController.h"
//#include "CharacterAllInterface.h"
//#include "GameFramework/Character.h"
//#include "Kismet/GameplayStatics.h"
//#include "BehaviorTree/BlackboardComponent.h"
//
//UBTTask_MoveToPlayer_Ranged::UBTTask_MoveToPlayer_Ranged()
//{
//    NodeName = TEXT("Move To Player_Ranged");
//}
//
//EBTNodeResult::Type UBTTask_MoveToPlayer_Ranged::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
//{
//    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
//
//    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
//    if (nullptr == ControllingPawn)
//    {
//        return EBTNodeResult::Failed;
//    }
//
//    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
//    if (BlackboardComp == nullptr)
//    {
//        return EBTNodeResult::Failed;
//    }
//
//    APawn* playerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
//    if (playerPawn == nullptr)
//    {
//        return EBTNodeResult::Failed;
//    }
//
//    float DistanceToPlayer = BlackboardComp->GetValueAsFloat(BBKEY_DISTANCE);
//    float MonsterCapsuleRadius = BlackboardComp->GetValueAsFloat(BBKEY_MONSTERRADIUS);
//    float PlayerCapsuleRadius = BlackboardComp->GetValueAsFloat(BBKEY_PLAYERRADIUS);
//
//    float MinDistance = 300.0f;
//
//    // 실제 최소 거리 계산
//    float CalculatedMinDistance = MinDistance + PlayerCapsuleRadius + MonsterCapsuleRadius;
//
//    FVector MonsterLocation = ControllingPawn->GetActorLocation();
//    FVector PlayerLocation = BlackboardComp->GetValueAsVector(BBKEY_PLAYERLOCATION);
//
//    if (DistanceToPlayer > CalculatedMinDistance)
//    {
//        OwnerComp.GetAIOwner()->MoveToActor(playerPawn, CalculatedMinDistance, false);
//    }
//    else
//    {
//        OwnerComp.GetAIOwner()->StopMovement();
//    }
//
//    return EBTNodeResult::Succeeded;
//}
//
//
////#include "BTTask_MoveToPlayer_Ranged.h"
////#include "MyAI.h"
////#include "AIController.h"
////#include "BehaviorTree/BlackboardComponent.h"
////#include "Kismet/GameplayStatics.h"
////#include "Navigation/PathFollowingComponent.h"
////#include "DrawDebugHelpers.h"
////
////UBTTask_MoveToPlayer_Ranged::UBTTask_MoveToPlayer_Ranged()
////{
////	NodeName = TEXT("Move To Player_Ranged (InProgress)");
////}
////
////EBTNodeResult::Type UBTTask_MoveToPlayer_Ranged::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
////{
////	Super::ExecuteTask(OwnerComp, NodeMemory);
////
////	AAIController* AICon = OwnerComp.GetAIOwner();
////	if (!AICon)
////	{
////		return EBTNodeResult::Failed;
////	}
////
////	APawn* MonsterPawn = AICon->GetPawn();
////	if (!MonsterPawn)
////	{
////		return EBTNodeResult::Failed;
////	}
////
////	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
////	if (!BlackboardComp)
////	{
////		return EBTNodeResult::Failed;
////	}
////
////	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
////	if (!PlayerPawn)
////	{
////		return EBTNodeResult::Failed;
////	}
////
////	// 블랙보드에서 값 읽기
////	float DistanceToPlayer = BlackboardComp->GetValueAsFloat(BBKEY_DISTANCE);
////	float MonsterCapsuleRadius = BlackboardComp->GetValueAsFloat(BBKEY_MONSTERRADIUS);
////	float PlayerCapsuleRadius = BlackboardComp->GetValueAsFloat(BBKEY_PLAYERRADIUS);
////
////	const float MinDistance = 300.0f;  // 기획상의 기본 최소 거리 (캡슐 표면 간)
////	// 실제 center-to-center로 유지해야 할 거리
////	float CalcMinDist = MinDistance + MonsterCapsuleRadius + PlayerCapsuleRadius;
////
////	// 만약 이미 거리가 충분히 가까워졌다면 (CalcMinDist 이하)
////	if (DistanceToPlayer <= CalcMinDist)
////	{
////		AICon->StopMovement();
////		return EBTNodeResult::Succeeded;
////	}
////
////	// 접근 태스크: 플레이어에게 접근 → MoveToActor 사용
////	// UE5.4의 MoveToActor 파라미터: (Goal, AcceptanceRadius, bStopOnOverlap, bUsePathfinding, bCanStrafe, FilterClass, bAllowPartialPath)
////	// bStopOnOverlap를 false로 전달합니다.
////	EPathFollowingRequestResult::Type ResultData = AICon->MoveToActor(
////		PlayerPawn,
////		CalcMinDist,
////		false,      // bStopOnOverlap = false
////		true,       // bUsePathfinding
////		true,       // bCanStrafe
////		nullptr,    // FilterClass
////		true        // bAllowPartialPath
////	);
////
////	if (ResultData == EPathFollowingRequestResult::RequestSuccessful)
////	{
////		CachedOwnerComp = &OwnerComp; // 콜백에서 사용
////		// 이동 완료 콜백 등록
////		AICon->GetPathFollowingComponent()->OnRequestFinished.AddUObject(
////			this,
////			&UBTTask_MoveToPlayer_Ranged::OnMoveCompleted
////		);
////		return EBTNodeResult::InProgress;
////	}
////	else
////	{
////		return EBTNodeResult::Failed;
////	}
////}
////
////EBTNodeResult::Type UBTTask_MoveToPlayer_Ranged::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
////{
////	AAIController* AICon = OwnerComp.GetAIOwner();
////	if (AICon && AICon->GetPathFollowingComponent())
////	{
////		AICon->GetPathFollowingComponent()->OnRequestFinished.RemoveAll(this);
////		AICon->StopMovement();
////	}
////	return EBTNodeResult::Aborted;
////}
////
////void UBTTask_MoveToPlayer_Ranged::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
////{
////	if (RequestID == MoveRequestID && CachedOwnerComp)
////	{
////		AAIController* AICon = CachedOwnerComp->GetAIOwner();
////		if (AICon && AICon->GetPathFollowingComponent())
////		{
////			AICon->GetPathFollowingComponent()->OnRequestFinished.RemoveAll(this);
////		}
////		if (Result.Code == EPathFollowingResult::Success)
////		{
////			FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
////		}
////		else
////		{
////			FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Failed);
////		}
////	}
////}
//
