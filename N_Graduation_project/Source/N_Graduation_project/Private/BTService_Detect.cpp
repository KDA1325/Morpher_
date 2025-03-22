//// Fill out your copyright notice in the Description page of Project Settings.
//
//
//#include "BTService_Detect.h"
//#include "MyAI.h"
//#include "AIController.h"
//#include "CharacterAllInterface.h"
//#include "Kismet/GameplayStatics.h"
//#include "BehaviorTree/BlackboardComponent.h"
//#include "DrawDebugHelpers.h"
//#include "Engine/OverlapResult.h"
//
//UBTService_Detect::UBTService_Detect()
//{
//	NodeName = TEXT("Detect");
//
//	// 1초 단위로 수행 
//	Interval = 0.1f;
//}
//
//void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
//{
//    // 디버그 모드 여부를 블랙보드나 AI 캐릭터 설정으로 관리
//    bool bShowDebug = true; // 필요에 따라 외부에서 설정할 수 있도록
//    
//    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
//
//    APawn* playerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
//    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
//    if (nullptr == ControllingPawn || nullptr == playerPawn)
//    {
//        return;
//    }
//
//    FVector Center = ControllingPawn->GetActorLocation();
//    UWorld* World = ControllingPawn->GetWorld();
//    FVector PlayerLocation = playerPawn->GetActorLocation();
//
//    float DistanceToPlayer = FVector::Dist(Center, PlayerLocation);
//
//    // 감지 범위 또는 최소 이동 거리 확인
//    float DetectRadius = 1000.0f;  // 감지 범위 (적절히 조정)
//    float MoveTriggerDistance = 200.0f;  // 이동 재개 거리
//
//
//    if (bShowDebug)
//    {
//        DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.2f);
//    }
//
//    if (DistanceToPlayer <= DetectRadius)
//    {
//        OwnerComp.GetBlackboardComponent()->SetValueAsObject(BBKEY_TARGET, playerPawn);
//        OwnerComp.GetBlackboardComponent()->SetValueAsVector(BBKEY_PLAYERLOCATION, PlayerLocation);
//        UE_LOG(LogTemp, Log, TEXT("Player detected at distance: %f"), DistanceToPlayer);
//    }
//    else
//    {
//        OwnerComp.GetBlackboardComponent()->ClearValue(BBKEY_TARGET);
//        UE_LOG(LogTemp, Log, TEXT("Player out of range"));
//    }
//}