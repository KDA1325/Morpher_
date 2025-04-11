// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FleeFromPlayer.h"
#include "MyAI.h"
#include "AIController.h"
#include "CharacterAllInterface.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FleeFromPlayer::UBTTask_FleeFromPlayer()
{
    NodeName = TEXT("FleeFromPlayer");
}

EBTNodeResult::Type UBTTask_FleeFromPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

    if (DistanceToPlayer < CalculatedMinDistance)
    {
        FVector FleeDirection = (MonsterLocation - PlayerLocation).GetSafeNormal();

        FVector FleeLocation = PlayerLocation + FleeDirection * CalculatedMinDistance;

        OwnerComp.GetAIOwner()->MoveToLocation(FleeLocation, 5.0f, false);
        //OwnerComp.GetAIOwner()->MoveToActor(playerPawn, CalculatedMinDistance, false);
    }
    else
    {
        OwnerComp.GetAIOwner()->StopMovement();
    }

    return EBTNodeResult::Succeeded;
}

//
//// Fill out your copyright notice in the Description page of Project Settings.
//
//#include "BTTask_FleeFromPlayer.h"
//#include "MyAI.h"
//#include "AIController.h"
//#include "CharacterAllInterface.h"
//#include "GameFramework/Character.h"
//#include "Kismet/GameplayStatics.h"
//#include "BehaviorTree/BlackboardComponent.h"
//
//UBTTask_FleeFromPlayer::UBTTask_FleeFromPlayer()
//{
//    NodeName = TEXT("FleeFromPlayer");
//}
//
//EBTNodeResult::Type UBTTask_FleeFromPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
//{
//    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
//
//    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
//    if (ControllingPawn == nullptr)
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
//    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
//    if (PlayerPawn == nullptr)
//    {
//        return EBTNodeResult::Failed;
//    }
//
//    float DistanceToPlayer = BlackboardComp->GetValueAsFloat(BBKEY_DISTANCE);
//    float MonsterCapsuleRadius = BlackboardComp->GetValueAsFloat(BBKEY_MONSTERRADIUS);
//    float PlayerCapsuleRadius = BlackboardComp->GetValueAsFloat(BBKEY_PLAYERRADIUS);
//
//    float MinDistance = 300.0f;
//    // 실제 최소 거리: 캡슐 표면 간 최소 거리 유지 → center-to-center 거리
//    float CalculatedMinDistance = MinDistance + PlayerCapsuleRadius + MonsterCapsuleRadius;
//
//    FVector MonsterLocation = ControllingPawn->GetActorLocation();
//    FVector PlayerLocation = BlackboardComp->GetValueAsVector(BBKEY_PLAYERLOCATION);
//
//    // 만약 플레이어와의 거리가 CalculatedMinDistance보다 작으면 도망 실행
//    if (DistanceToPlayer < CalculatedMinDistance)
//    {
//        FVector FleeDirection = (MonsterLocation - PlayerLocation).GetSafeNormal();
//        FVector FleeLocation = PlayerLocation + FleeDirection * CalculatedMinDistance;
//        OwnerComp.GetAIOwner()->MoveToLocation(FleeLocation, 5.0f, false);
//    }
//    else
//    {
//        OwnerComp.GetAIOwner()->StopMovement();
//    }
//
//    return EBTNodeResult::Succeeded;
//}
//
////// Fill out your copyright notice in the Description page of Project Settings.
////
////
////#include "BTTask_FleeFromPlayer.h"
////#include "MyAI.h"
////#include "AIController.h"
////#include "CharacterAllInterface.h"
////#include "GameFramework/Character.h"
////#include "Kismet/GameplayStatics.h"
////#include "BehaviorTree/BlackboardComponent.h"
////
////UBTTask_FleeFromPlayer::UBTTask_FleeFromPlayer()
////{
////    NodeName = TEXT("FleeFromPlayer");
////}
////
////EBTNodeResult::Type UBTTask_FleeFromPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
////{
////    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
////
////    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
////    if (nullptr == ControllingPawn)
////    {
////        return EBTNodeResult::Failed;
////    }
////
////    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
////    if (BlackboardComp == nullptr)
////    {
////        return EBTNodeResult::Failed;
////    }
////
////    APawn* playerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
////    if (playerPawn == nullptr)
////    {
////        return EBTNodeResult::Failed;
////    }
////
////    float DistanceToPlayer = BlackboardComp->GetValueAsFloat(BBKEY_DISTANCE);
////    float MonsterCapsuleRadius = BlackboardComp->GetValueAsFloat(BBKEY_MONSTERRADIUS);
////    float PlayerCapsuleRadius = BlackboardComp->GetValueAsFloat(BBKEY_PLAYERRADIUS);
////
////    float MinDistance = 300.0f;
////
////    // 실제 최소 거리 계산
////    float CalculatedMinDistance = MinDistance + PlayerCapsuleRadius + MonsterCapsuleRadius;
////
////    FVector MonsterLocation = ControllingPawn->GetActorLocation();
////    FVector PlayerLocation = BlackboardComp->GetValueAsVector(BBKEY_PLAYERLOCATION);
////
////    if (DistanceToPlayer < CalculatedMinDistance)
////    {
////        FVector FleeDirection = (MonsterLocation - PlayerLocation).GetSafeNormal();
////
////        FVector FleeLocation = PlayerLocation + FleeDirection * CalculatedMinDistance;
////
////        OwnerComp.GetAIOwner()->MoveToLocation(FleeLocation, 5.0f, false);
////        //OwnerComp.GetAIOwner()->MoveToActor(playerPawn, CalculatedMinDistance, false);
////    }
////    else
////    {
////        OwnerComp.GetAIOwner()->StopMovement();
////    }
////
////    return EBTNodeResult::Succeeded;
////}
////
