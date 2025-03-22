//// Fill out your copyright notice in the Description page of Project Settings.
//
//
//#include "BTService_MoveToPlayer.h"
//#include "MyAI.h"
//#include "AIController.h"
//#include "CharacterAllInterface.h"
//#include "Kismet/GameplayStatics.h"
//#include "BehaviorTree/BlackboardComponent.h"
//#include "DrawDebugHelpers.h"
//#include "Engine/OverlapResult.h"
//
//UBTService_MoveToPlayer::UBTService_MoveToPlayer()
//{
//	NodeName = TEXT("Detect");
//
//	// 1초 단위로 수행 
//	Interval = 1.0f;
//}
//
//void UBTService_MoveToPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
//{
//	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
//
//	APawn* playerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
//
//	// 지금 제어하고 있는 폰 정보 가져옴
//	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
//	if (nullptr == ControllingPawn)
//	{
//		return;
//	}
//
//	// 폰의 위치와 폰이 속한 월드의 값 가져옴 
//	FVector Center = ControllingPawn->GetActorLocation();
//	UWorld* World = ControllingPawn->GetWorld();
//	if (nullptr == World)
//	{
//		return;
//	}
//
//	// AI 인터페이스 형 변환
//	ICharacterAllInterface* AIPawn = Cast<ICharacterAllInterface>(ControllingPawn);
//	if (nullptr == AIPawn)
//	{
//		return;
//	}
//
//	//OwnerComp.GetBlackboardComponent()->SetValueAsVector(BBKEY_PLAYERLOCATION, playerPawn->GetActorLocation());
//
//	// 인터페이스로부터 감지할 영역에 대한 값 가져옴 
//	float DetectRadius = AIPawn->GetAIDetectRange();
//
//	// 멀티 함수의 결과물이 저장되는 배열 
//	TArray<FOverlapResult> OverlapResults;
//	FCollisionQueryParams CollisionQueryParam(SCENE_QUERY_STAT(Detect), false, ControllingPawn);
//
//	// 플레이어가 다수 있다는 가정 하에 사용하는 멀티 함수 
//	bool bResult = World->OverlapMultiByChannel(
//		OverlapResults,
//		Center,
//		FQuat::Identity,
//		ECC_GameTraceChannel1, // ECC_Visibility, //ECC_GameTraceChannel1,
//		FCollisionShape::MakeSphere(DetectRadius),
//		CollisionQueryParam
//	);
//
//	// 멀티 함수를 통해 감지된 폰이 있다면 
//	if (bResult)
//	{
//		// 감지된 모든 폰에 대해 검사 수행 
//		for (auto const& OverlapResult : OverlapResults)
//		{
//			APawn* Pawn = Cast<APawn>(OverlapResult.GetActor());
//
//			// 감지된 폰을 조종하고 있는 것이 플레이어 컨트롤러일 경우 (= 플레이어 캐릭터일 경우) 
//			if (Pawn && Pawn->GetController()->IsPlayerController())
//			{
//				// 폰의 이름을 로그로 출력
//				UE_LOG(LogTemp, Log, TEXT("Detected Pawn Name: %s"), *Pawn->GetName());
//
//				// BBKEY_TARGET을 폰으로 해서 타겟값 저장 
//				OwnerComp.GetBlackboardComponent()->SetValueAsObject(BBKEY_TARGET, Pawn);
//				DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.2f);
//
//				// 플레이어의 위치를 선과 점으로 표현 
//				DrawDebugPoint(World, Pawn->GetActorLocation(), 10.0f, FColor::Green, false, 0.2f);
//				DrawDebugLine(World, ControllingPawn->GetActorLocation(), Pawn->GetActorLocation(), FColor::Green, false, 0.27f);
//				return;
//			}
//		}
//	}
//
//	// 폰을 감지했지만 플레이어를 찾지 못한 경우 or 폰을 아예 감지하지 못한 경우 
//	// 타겟값: null
//	OwnerComp.GetBlackboardComponent()->SetValueAsObject(BBKEY_TARGET, nullptr);
//	DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.2f);
//
//
//	APawn* TargetPawn = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKEY_TARGET));
//	if (nullptr == TargetPawn)
//	{
//		OwnerComp.GetAIOwner()->StopMovement();
//		return;
//	}
//
//
//	// ControllingPawn과 플레이어 사이의 거리 저장 
//	float DistanceToPlayer = FVector::Dist(ControllingPawn->GetActorLocation(), TargetPawn->GetActorLocation());
//
//	// 최소 거리 
//	float MinDistance = 100.0f;
//
//	// ControllingPawn과 플레이어 사이의 거리는 최소 거리 이상으로 가까워지지 않는다
//	if (DistanceToPlayer >=  MinDistance)
//	{
//		// AI Owner 액터가 playerPawn을 목표지점으로, MinDistance만큼 거리까지 이동, Overlap되면 stop
//		OwnerComp.GetAIOwner()->MoveToActor(TargetPawn, MinDistance, true);
//	}
//	else
//	{
//		OwnerComp.GetAIOwner()->StopMovement();
//	}
//}
//
//
////// Fill out your copyright notice in the Description page of Project Settings.
////
////
////#include "BTService_MoveToPlayer.h"
////#include "MyAI.h"
////#include "AIController.h"
////#include "CharacterAllInterface.h"
////#include "Kismet/GameplayStatics.h"
////#include "GameFramework/Character.h"
////#include "BehaviorTree/BlackboardComponent.h"
////#include "DrawDebugHelpers.h"
////#include "Engine/OverlapResult.h"
////
////UBTService_MoveToPlayer::UBTService_MoveToPlayer()
////{
////	NodeName = TEXT("Move To Player");
////
////	// 0.1초 단위로 수행 
////	Interval = 1.0f;
////}
////
////void UBTService_MoveToPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
////{
////	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
////
////	//APawn* playerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
////
////	// 지금 제어하고 있는 폰 정보 가져옴
////	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
////	if (nullptr == ControllingPawn)
////	{
////		return;
////	}
////
////	// 폰의 위치와 폰이 속한 월드의 값 가져옴 
////	FVector Center = ControllingPawn->GetActorLocation();
////	UWorld* World = ControllingPawn->GetWorld();
////	if (nullptr == World)
////	{
////		return;
////	}
////
////	// AI 인터페이스 형 변환
////	ICharacterAllInterface* AIPawn = Cast<ICharacterAllInterface>(ControllingPawn);
////	if (nullptr == AIPawn)
////	{
////		return;
////	}
////
////	// 플레이어 가져오기
////	//ACharacter* PlayerCharacter = Cast<ACharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKEY_TARGET));
////	//if (nullptr == PlayerCharacter)
////	//{
////	//	return;
////	//}
////
////	//OwnerComp.GetBlackboardComponent()->SetValueAsVector(BBKEY_PLAYERLOCATION, playerPawn->GetActorLocation());
////
////	// 인터페이스로부터 감지할 영역에 대한 값 가져옴 
////	float DetectRadius = AIPawn->GetAIDetectRange();
////
////	// 멀티 함수의 결과물이 저장되는 배열 
////	TArray<FOverlapResult> OverlapResults;
////	FCollisionQueryParams CollisionQueryParam(SCENE_QUERY_STAT(Detect), false, ControllingPawn);
////
////	// 플레이어가 다수 있다는 가정 하에 사용하는 멀티 함수 
////	bool bResult = World->OverlapMultiByChannel(
////		OverlapResults,
////		Center,
////		FQuat::Identity,
////		ECC_GameTraceChannel1, // ECC_Visibility, //ECC_GameTraceChannel1,
////		FCollisionShape::MakeSphere(DetectRadius),
////		CollisionQueryParam
////	);
////
////	// 멀티 함수를 통해 감지된 폰이 있다면 
////	if (bResult)
////	{
////		// 감지된 모든 폰에 대해 검사 수행 
////		for (auto const& OverlapResult : OverlapResults)
////		{
////			APawn* Pawn = Cast<APawn>(OverlapResult.GetActor());
////
////			// 감지된 폰을 조종하고 있는 것이 플레이어 컨트롤러일 경우 (= 플레이어 캐릭터일 경우) 
////			if (Pawn && Pawn->GetController()->IsPlayerController())
////			{
////				// 폰의 이름을 로그로 출력
////				UE_LOG(LogTemp, Log, TEXT("Detected Pawn Name: %s"), *Pawn->GetName());
////
////				// BBKEY_TARGET을 폰으로 해서 타겟값 저장 
////				OwnerComp.GetBlackboardComponent()->SetValueAsObject(BBKEY_TARGET, Pawn);
////				DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.2f);
////
////				// 플레이어의 위치를 선과 점으로 표현 
////				DrawDebugPoint(World, Pawn->GetActorLocation(), 10.0f, FColor::Green, false, 0.2f);
////				DrawDebugLine(World, ControllingPawn->GetActorLocation(), Pawn->GetActorLocation(), FColor::Green, false, 0.27f);
////				return;
////			}
////		}
////	}
////	else
////	{
////		// 폰을 감지했지만 플레이어를 찾지 못한 경우 or 폰을 아예 감지하지 못한 경우 
////		// 타겟값: null
////		OwnerComp.GetBlackboardComponent()->SetValueAsObject(BBKEY_TARGET, nullptr);
////		DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.2f);
////	}
////
////	//// ControllingPawn과 플레이어 사이의 거리 저장 
////	//float DistanceToPlayer = FVector::Dist(ControllingPawn->GetActorLocation(), PlayerCharacter->GetActorLocation());
////
////	//// 최소 거리 
////	//float MinDistance = 500.0f;
////
////	//// ControllingPawn과 플레이어 사이의 거리는 최소 거리 이상으로 가까워지지 않는다
////	//if (DistanceToPlayer >=  MinDistance)
////	//{
////	//	// AI Owner 액터가 PlayerCharacter를 목표지점으로, MinDistance만큼 거리까지 이동, Overlap되면 stop
////	//	OwnerComp.GetAIOwner()->MoveToActor(PlayerCharacter, MinDistance, true);
////	//}
////	//else
////	//{
////	//	OwnerComp.GetAIOwner()->StopMovement();
////	//}
////
////	
////}
