// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_MoveToPlayer.h"
#include "MyAI.h"
#include "AIController.h"
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

    float DistanceToPlayer = BlackboardComp->GetValueAsFloat(BBKEY_DISTANCE);
    float MinDistance = BlackboardComp->GetValueAsFloat(BBKEY_MINDISTANCE);
    float MonsterCapsuleRadius = BlackboardComp->GetValueAsFloat(BBKEY_MONSTERRADIUS);
    float PlayerCapsuleRadius = BlackboardComp->GetValueAsFloat(BBKEY_PLAYERRADIUS);

    // 실제 최소 거리 계산
    float CalculatedMinDistance = MinDistance + PlayerCapsuleRadius + MonsterCapsuleRadius;

    if (DistanceToPlayer >= CalculatedMinDistance)
    {
        FVector PlayerLocation = BlackboardComp->GetValueAsVector(BBKEY_PLAYERLOCATION);
        OwnerComp.GetAIOwner()->MoveToActor(playerPawn, CalculatedMinDistance, false);
        return EBTNodeResult::Succeeded;
    }
    else
    {
        OwnerComp.GetAIOwner()->StopMovement();
    }

    return EBTNodeResult::Succeeded;
}

//EBTNodeResult::Type UBTTask_MoveToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
//{
//    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
//
//    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
//    if (nullptr == ControllingPawn)
//    {
//        return EBTNodeResult::Failed;
//    }
//
//    ACharacter* PlayerCharacter = Cast<ACharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKEY_TARGET));
//    if (nullptr == PlayerCharacter)
//    {
//        return EBTNodeResult::Failed;
//    }
//
//    float PlayerRadius = PlayerCharacter->GetSimpleCollisionRadius();
//    float MonsterRadius = ControllingPawn->GetSimpleCollisionRadius();
//    float MinDistance = 30.0f + PlayerRadius + MonsterRadius;
//
//    // 이동 명령 요청
//    FAIRequestID MoveRequestID = OwnerComp.GetAIOwner()->MoveToActor(PlayerCharacter, MinDistance, true);
//
//    if (MoveRequestID.IsValid())
//    {
//        return EBTNodeResult::InProgress;
//    }
//
//    return EBTNodeResult::Failed;
//
//    //// 이동 명령 요청
//    //OwnerComp.GetAIOwner()->MoveToActor(PlayerCharacter, MinDistance, true);
//
//    //return EBTNodeResult::InProgress;
//
//    //// 거리 계산
//    //float DistanceToPlayer = FVector::Dist(ControllingPawn->GetActorLocation(), PlayerCharacter->GetActorLocation());
//
//    // 디버그 드로잉
//    UWorld* World = ControllingPawn->GetWorld();
//    if (World)
//    {
//        DrawDebugLine(World, ControllingPawn->GetActorLocation(), PlayerCharacter->GetActorLocation(), FColor::Red, false, 0.5f, 0, 5.0f);
//        DrawDebugSphere(World, PlayerCharacter->GetActorLocation(), 30.0f, 16, FColor::Blue, false, 0.5f);
//        //UE_LOG(LogTemp, Log, TEXT("Move To Player - Distance: %f, MinDistance: %f"), DistanceToPlayer, MinDistance);
//    }
//
//    //// 이동 명령 요청
//    //FAIRequestID MoveRequestID = OwnerComp.GetAIOwner()->MoveToActor(PlayerCharacter, MinDistance, true);
//
//    //// 이동 명령이 성공적으로 수행되었든 아니든 태스크를 종료하여 다시 실행할 수 있도록 설정
//    //if (MoveRequestID.IsValid())
//    //{
//    //    UE_LOG(LogTemp, Log, TEXT("Moving to player..."));
//    //    return EBTNodeResult::InProgress;
//    //}
//
//    //OwnerComp.GetAIOwner()->StopMovement();
//    //return EBTNodeResult::Failed;
//}
//void UBTTask_MoveToPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
//{
//    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
//
//    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
//    if (nullptr == ControllingPawn)
//    {
//        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
//        return;
//    }
//
//    ACharacter* PlayerCharacter = Cast<ACharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKEY_TARGET));
//    if (nullptr == PlayerCharacter)
//    {
//        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
//        return;
//    }
//
//    float DistanceToPlayer = FVector::Dist(ControllingPawn->GetActorLocation(), PlayerCharacter->GetActorLocation());
//
//    float PlayerRadius = PlayerCharacter->GetSimpleCollisionRadius();
//    float MonsterRadius = ControllingPawn->GetSimpleCollisionRadius();
//    float MinDistance = 30.0f + PlayerRadius + MonsterRadius;
//
//    // 디버그 로그 추가
//    UE_LOG(LogTemp, Log, TEXT("[%s] DistanceToPlayer: %f, MinDistance: %f"), *ControllingPawn->GetName(), DistanceToPlayer, MinDistance);
//
//    // 이미 도착했거나 이동 중이면 불필요한 이동 명령 방지
//    if (OwnerComp.GetAIOwner()->GetMoveStatus() == EPathFollowingStatus::Moving)
//    {
//        return;
//    }
//
//    if (DistanceToPlayer <= MinDistance)
//    {
//        OwnerComp.GetAIOwner()->StopMovement();
//        UE_LOG(LogTemp, Log, TEXT("[%s] Arrived at player. Stopping movement."), *ControllingPawn->GetName());
//        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
//        return;
//    }
//    else
//    {
//        UE_LOG(LogTemp, Log, TEXT("[%s] Moving towards player..."), *ControllingPawn->GetName());
//        OwnerComp.GetAIOwner()->MoveToActor(PlayerCharacter, MinDistance, true);
//    }
//}
////
////void UBTTask_MoveToPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
////{
////    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
////
////    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
////    if (nullptr == ControllingPawn)
////    {
////        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
////        return;
////    }
////
////    ACharacter* PlayerCharacter = Cast<ACharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKEY_TARGET));
////    if (nullptr == PlayerCharacter)
////    {
////        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
////        return;
////    }
////
////    float DistanceToPlayer = FVector::Dist(ControllingPawn->GetActorLocation(), PlayerCharacter->GetActorLocation());
////
////    // 근접 거리 도달 여부 판단
////    float PlayerRadius = PlayerCharacter->GetSimpleCollisionRadius();
////    float MonsterRadius = ControllingPawn->GetSimpleCollisionRadius();
////    float MinDistance = 30.0f + PlayerRadius + MonsterRadius;
////
////    // 디버그 로그 추가
////    UE_LOG(LogTemp, Log, TEXT("DistanceToPlayer: %f, MinDistance: %f"), DistanceToPlayer, MinDistance);
////
////
////    if (DistanceToPlayer <= MinDistance)
////    {
////        // 목표 위치에 도달하면 멈추고 태스크 성공 처리
////        OwnerComp.GetAIOwner()->StopMovement();
////        UE_LOG(LogTemp, Log, TEXT("Arrived at player. Stopping movement."));
////        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
////        return;
////
////        //// 목표 위치에 도달하면 멈추고 태스크를 계속 유지
////        //OwnerComp.GetAIOwner()->StopMovement();
////    }
////
////    // 이동 중이 아닌 경우 이동 명령 재요청
////    if (OwnerComp.GetAIOwner()->GetMoveStatus() != EPathFollowingStatus::Moving)
////    {
////        UE_LOG(LogTemp, Log, TEXT("Resuming movement towards player."));
////        OwnerComp.GetAIOwner()->MoveToActor(PlayerCharacter, MinDistance, true);
////    }
////    //else
////    //{
////    //    // 이동 상태가 아니라면 다시 이동 명령 요청
////    //    if (OwnerComp.GetAIOwner()->GetMoveStatus() != EPathFollowingStatus::Moving)
////    //    {
////    //        OwnerComp.GetAIOwner()->MoveToActor(PlayerCharacter, MinDistance, true);
////    //    }
////    //}
////
////    //if (DistanceToPlayer > MinDistance)
////    //{
////    //    // 이동 상태가 아니면 다시 이동 명령
////    //    if (OwnerComp.GetAIOwner()->GetMoveStatus() != EPathFollowingStatus::Moving)
////    //    {
////    //        OwnerComp.GetAIOwner()->MoveToActor(PlayerCharacter, MinDistance, true);
////    //    }
////    //}
////    //else
////    //{
////    //    // 멈춤 명령
////    //    OwnerComp.GetAIOwner()->StopMovement();
////    //    // 태스크 종료하지 않고 InProgress 유지
////    //}
////
////
////    //// 디버그 로그와 시각화 추가
////    //UE_LOG(LogTemp, Log, TEXT("Tick - DistanceToPlayer: %f, MinDistance: %f"), DistanceToPlayer, MinDistance);
////
////    //if (DistanceToPlayer <= MinDistance)
////    //{
////    //    // 목표 위치에 도달하면 멈추고 태스크를 계속 유지
////    //    OwnerComp.GetAIOwner()->StopMovement();
////    //}
////    //else
////    //{
////    //    // 이동 상태가 아니라면 다시 이동 명령 요청
////    //    if (OwnerComp.GetAIOwner()->GetMoveStatus() != EPathFollowingStatus::Moving)
////    //    {
////    //        OwnerComp.GetAIOwner()->MoveToActor(PlayerCharacter, MinDistance, true);
////    //    }
////    //}
////
////    //if (DistanceToPlayer <= MinDistance)
////    //{
////    //    // 도착 처리
////    //    OwnerComp.GetAIOwner()->StopMovement();
////    //    FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
////    //}
////    //else
////    //{
////    //    // 이동 상태 확인
////    //    if (OwnerComp.GetAIOwner()->GetMoveStatus() != EPathFollowingStatus::Moving)
////    //    {
////    //        UE_LOG(LogTemp, Log, TEXT("Restarting movement to player..."));
////    //        OwnerComp.GetAIOwner()->MoveToActor(PlayerCharacter, MinDistance, true);
////    //    }
////    //}
////}
//
//
////// Fill out your copyright notice in the Description page of Project Settings.
////
////
////#include "BTTask_MoveToPlayer.h"
////#include "MyAI.h"
////#include "AIController.h"
////#include "CharacterAllInterface.h"
////#include "GameFramework/Character.h"
////#include "Kismet/GameplayStatics.h"
////#include "BehaviorTree/BlackboardComponent.h"
////#include "Navigation/PathFollowingComponent.h"
////
////UBTTask_MoveToPlayer::UBTTask_MoveToPlayer()
////{
////	NodeName = TEXT("Move To Player");
////}
////
////EBTNodeResult::Type UBTTask_MoveToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
////{
////    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
////    if (nullptr == ControllingPawn)
////    {
////        return EBTNodeResult::Failed;
////    }
////
////    ACharacter* PlayerCharacter = Cast<ACharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKEY_TARGET));
////    if (nullptr == PlayerCharacter)
////    {
////        return EBTNodeResult::Failed;
////    }
////
////    float PlayerRadius = PlayerCharacter->GetSimpleCollisionRadius();
////    float MonsterRadius = ControllingPawn->GetSimpleCollisionRadius();
////    float MinDistance = 30.0f + PlayerRadius + MonsterRadius;
////
////    // 이동 명령 요청
////    FAIRequestID MoveRequestID = OwnerComp.GetAIOwner()->MoveToActor(PlayerCharacter, MinDistance, true);
////
////    // 이동 명령이 성공적으로 수행되었든 아니든 태스크를 종료하여 다시 실행할 수 있도록 설정
////    if (MoveRequestID.IsValid())
////    {
////        UE_LOG(LogTemp, Log, TEXT("Moving to player..."));
////        return EBTNodeResult::Succeeded;
////    }
////
////    OwnerComp.GetAIOwner()->StopMovement();
////    return EBTNodeResult::Failed;
////}
////
////void UBTTask_MoveToPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
////{
////    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
////
////    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
////    if (nullptr == ControllingPawn)
////    {
////        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
////        return;
////    }
////
////    ACharacter* PlayerCharacter = Cast<ACharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKEY_TARGET));
////    if (nullptr == PlayerCharacter)
////    {
////        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
////        return;
////    }
////
////    float DistanceToPlayer = FVector::Dist(ControllingPawn->GetActorLocation(), PlayerCharacter->GetActorLocation());
////
////    // 근접 거리 도달 여부 판단
////    float PlayerRadius = PlayerCharacter->GetSimpleCollisionRadius();
////    float MonsterRadius = ControllingPawn->GetSimpleCollisionRadius();
////    float MinDistance = 30.0f + PlayerRadius + MonsterRadius;
////
////    if (DistanceToPlayer <= MinDistance)
////    {
////        // 목표 위치에 도달하면 멈추고 태스크를 계속 유지
////        OwnerComp.GetAIOwner()->StopMovement();
////    }
////    else
////    {
////        // 이동 상태가 아니라면 다시 이동 명령 요청
////        if (OwnerComp.GetAIOwner()->GetMoveStatus() != EPathFollowingStatus::Moving)
////        {
////            OwnerComp.GetAIOwner()->MoveToActor(PlayerCharacter, MinDistance, true);
////        }
////    }
////}
//
////// Fill out your copyright notice in the Description page of Project Settings.
////
////
////#include "BTTask_MoveToPlayer.h"
////#include "MyAI.h"
////#include "AIController.h"
////#include "CharacterAllInterface.h"
////#include "GameFramework/Character.h"
////#include "Kismet/GameplayStatics.h"
////#include "BehaviorTree/BlackboardComponent.h"
////#include "Navigation/PathFollowingComponent.h"
////
////UBTTask_MoveToPlayer::UBTTask_MoveToPlayer()
////{
////	NodeName = TEXT("Move To Player");
////}
////
////EBTNodeResult::Type UBTTask_MoveToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
////{
////	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
////
////	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
////
////	if (nullptr == ControllingPawn)
////	{
////		return EBTNodeResult::Failed;
////	}
////
////	ICharacterAllInterface* AIPawn = Cast<ICharacterAllInterface>(ControllingPawn);
////	if (nullptr == AIPawn)
////	{
////		return EBTNodeResult::Failed;
////	}
////
////	// 플레이어 가져오기
////	APawn* playerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
////
////	ACharacter* PlayerCharacter = Cast<ACharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKEY_TARGET));
////	if (nullptr == PlayerCharacter)
////	{
////		return EBTNodeResult::Failed;
////	}
////
////	FVector playerLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(BBKEY_PLAYERLOCATION);
////
////	// ControllingPawn과 플레이어 사이의 거리 저장 
////	float DistanceToPlayer = FVector::Dist(ControllingPawn->GetActorLocation(), playerLocation);
////	//float DistanceToPlayer = FVector::Dist(ControllingPawn->GetActorLocation(), PlayerCharacter->GetActorLocation());
////
////	// PC 캡슐 반지름
////	float PlayerRadius = PlayerCharacter->GetSimpleCollisionRadius();
////
////	// 몬스터 캡슐 반지름
////	float MonsterRadius = ControllingPawn->GetSimpleCollisionRadius();
////	
////	// 최소 거리 (이 거리는 반드시 유지)
////	float MinDistance = 30.0f + PlayerRadius + MonsterRadius;
////
////	//// ControllingPawn과 플레이어 사이의 거리는 최소 거리 이상으로 가까워지지 않는다
////	//if (DistanceToPlayer > MinDistance)
////	//{
////	//	// AI Owner 액터가 PlayerCharacter를 목표지점으로, MinDistance만큼 거리까지 이동, Overlap되면 stop
////	//	EPathFollowingRequestResult::Type MoveResult = OwnerComp.GetAIOwner()->MoveToActor(PlayerCharacter, MinDistance, true);
////
////	//	// 이동 상태 확인
////	//	if (OwnerComp.GetAIOwner()->GetPathFollowingComponent()->GetStatus() == EPathFollowingStatus::Moving)
////	//	{
////	//		UE_LOG(LogTemp, Log, TEXT("Moving to player..."));
////	//		return EBTNodeResult::InProgress;
////	//	}
////
////	//	UE_LOG(LogTemp, Warning, TEXT("Failed to move to player"));
////	//	return EBTNodeResult::Failed;
////
////	//	// AI Owner 액터가 PlayerCharacter를 목표지점으로, MinDistance만큼 거리까지 이동, Overlap되면 stop
////	//	//OwnerComp.GetAIOwner()->MoveToActor(PlayerCharacter, MinDistance, true);
////
////	//	//return EBTNodeResult::InProgress;
////	//}
////	//
////	//// 최소 거리를 유지하지 못하고 가까워졌다면 이동 중지 
////	//OwnerComp.GetAIOwner()->StopMovement();
////
////	//return EBTNodeResult::Succeeded;
////	// 이미 이동 중이면 중복 명령 방지
////	if (OwnerComp.GetAIOwner()->GetMoveStatus() == EPathFollowingStatus::Moving)
////	{
////		return EBTNodeResult::InProgress;
////	}
////
////	FAIRequestID MoveRequestID = OwnerComp.GetAIOwner()->MoveToActor(PlayerCharacter, MinDistance, true);
////
////	if (MoveRequestID.IsValid())
////	{
////		return EBTNodeResult::InProgress;
////	}
////	else
////	{
////		OwnerComp.GetAIOwner()->StopMovement();
////		return EBTNodeResult::Failed;
////	}
////}
////
////void UBTTask_MoveToPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
////{
////	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
////
////	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
////	if (nullptr == ControllingPawn)
////	{
////		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
////		return;
////	}
////
////	ACharacter* PlayerCharacter = Cast<ACharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKEY_TARGET));
////	if (nullptr == PlayerCharacter)
////	{
////		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
////		return;
////	}
////
////	float DistanceToPlayer = FVector::Dist(ControllingPawn->GetActorLocation(), PlayerCharacter->GetActorLocation());
////
////	float PlayerRadius = PlayerCharacter->GetSimpleCollisionRadius();
////	float MonsterRadius = ControllingPawn->GetSimpleCollisionRadius();
////	float MinDistance = 30.0f + PlayerRadius + MonsterRadius;
////
////	if (DistanceToPlayer > MinDistance)
////	{
////		OwnerComp.GetAIOwner()->MoveToActor(PlayerCharacter, MinDistance, true);
////	}
////	else
////	{
////		OwnerComp.GetAIOwner()->StopMovement();
////		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
////	}
////}