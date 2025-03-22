// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_UpdatePlayerLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "MyAI.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"

UBTService_UpdatePlayerLocation::UBTService_UpdatePlayerLocation()
{
	NodeName = TEXT("Update Player Location");
	Interval = 0.1f;
}

void UBTService_UpdatePlayerLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* playerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (playerPawn == nullptr)
	{
		return;
	}

	// 지금 제어하고 있는 폰 정보 가져옴
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn)
	{
		return;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp == nullptr)
	{
		return;
	}

	// 충돌 캡슐 반지름 (PC와 몬스터의 캡슐 반지름을 더해서 최소 거리 계산)
	UCapsuleComponent* PlayerCapsuleComp = playerPawn->FindComponentByClass<UCapsuleComponent>();
	float PlayerCapsuleRadius = PlayerCapsuleComp->GetScaledCapsuleRadius();
	BlackboardComp->SetValueAsFloat(BBKEY_PLAYERRADIUS, PlayerCapsuleRadius);


	UCapsuleComponent* CapsuleComp = ControllingPawn->FindComponentByClass<UCapsuleComponent>();
	float MonsterCapsuleRadius = CapsuleComp->GetScaledCapsuleRadius();
	BlackboardComp->SetValueAsFloat(BBKEY_MONSTERRADIUS, MonsterCapsuleRadius);

    FVector AILocation = ControllingPawn->GetActorLocation();
    FVector PlayerLocation = playerPawn->GetActorLocation();

    float DistanceToPlayer = FVector::Dist(AILocation, PlayerLocation);

	if (BlackboardComp == nullptr)
	{
		return;
	}
	else 
	{
		BlackboardComp->SetValueAsVector(BBKEY_PLAYERLOCATION, PlayerLocation);
		BlackboardComp->SetValueAsFloat(BBKEY_DISTANCE, DistanceToPlayer);
	}

	// 최소 거리
	float MinDistance = BlackboardComp->GetValueAsFloat(BBKEY_MINDISTANCE);

	// 실제 최소 거리 계산
	float CalculatedMinDistance = MinDistance + PlayerCapsuleRadius + MonsterCapsuleRadius;

	UWorld* World = ControllingPawn->GetWorld();

	// 최소 거리 원 시각화 (몬스터 기준)
	DrawDebugSphere(
		World,
		ControllingPawn->GetActorLocation(),
		CalculatedMinDistance,
		8,
		FColor::Blue,
		false,
		0.5f
	);

	// PC와 몬스터 사이의 거리 시각화
	DrawDebugLine(
		World,
		ControllingPawn->GetActorLocation(),
		PlayerLocation,
		(DistanceToPlayer >= CalculatedMinDistance) ? FColor::Green : FColor::Red,
		false,
		0.1f,
		0,
		3.0f
	);

	// 거리 값 시각화
	FVector MidPoint = (ControllingPawn->GetActorLocation() + PlayerLocation) * 0.5f;
	DrawDebugString(
		World,
		MidPoint,
		FString::Printf(TEXT("Distance: %.1f / Min: %.1f"), DistanceToPlayer, CalculatedMinDistance),
		nullptr,
		FColor::White,
		0.1f,
		false
	);
}
