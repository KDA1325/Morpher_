// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_UpdatePlayerLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "MyAI.h"
#include "AIController.h"
#include "ABEntityData.h"
#include "EntityPreset.h"
#include "EntitySkillComponent.h"
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

	BlackboardComp->SetValueAsVector(BBKEY_PLAYERLOCATION, PlayerLocation);
	BlackboardComp->SetValueAsFloat(BBKEY_DISTANCE, DistanceToPlayer);

	// Blackboard로부터 값 읽어오기
	float A_SkillRange = BlackboardComp->GetValueAsFloat(BBKEY_ASKILLRANGE);
	float B_SkillRange = BlackboardComp->GetValueAsFloat(BBKEY_BSKILLRANGE);

	bool bA_SkillAvailable = BlackboardComp->GetValueAsBool(BBKEY_BASKILLAVAILABLE);
	bool bB_SkillAvailable = BlackboardComp->GetValueAsBool(BBKEY_BBSKILLAVAILABLE);

	// 거리 조건 계산: 조건을 매 틱마다 새로 설정
	bool bASkillCondition = (DistanceToPlayer <= A_SkillRange) && bA_SkillAvailable;
	bool bBSkillCondition = ((DistanceToPlayer > A_SkillRange) && (DistanceToPlayer <= B_SkillRange)) && bB_SkillAvailable;

	// 블랙보드에 조건 결과 업데이트
	BlackboardComp->SetValueAsBool(BBKEY_BASKILLCONDITION, bASkillCondition);
	BlackboardComp->SetValueAsBool(BBKEY_BBSKILLCONDITION, bBSkillCondition);

	UE_LOG(LogTemp, Warning, TEXT("bASkillCondition: %s"), bASkillCondition ? TEXT("True") : TEXT("False"));
	UE_LOG(LogTemp, Warning, TEXT("bBSkillCondition: %s"), bBSkillCondition ? TEXT("True") : TEXT("False"));
	
	AEntityPreset* Entity = Cast<AEntityPreset>(ControllingPawn);

	// 만약 현재 엔티티가 스킬 시전 중이라면, SelectedSkillID 등 업데이트를 건너뛰고 리턴
	//if (!Entity->bIsCastingSkill)
	//{
	//	//// 거리 기반 스킬 조건 계산 및 SelectedSkillID 설정
	//	//BlackboardComp->SetValueAsBool(BBKEY_BASKILLCONDITION, bASkillCondition);
	//	//BlackboardComp->SetValueAsBool(BBKEY_BBSKILLCONDITION, bBSkillCondition);

	//	//if (bASkillCondition)
	//	//	BlackboardComp->SetValueAsString(BBKEY_SELECTEDSKILLID, TEXT("Skill_Bite"));
	//	//else if (bBSkillCondition)
	//	//	BlackboardComp->SetValueAsString(BBKEY_SELECTEDSKILLID, TEXT("Skill_Charge"));
	//	//else
	//	//	BlackboardComp->ClearValue(BBKEY_SELECTEDSKILLID);
	//	
	//	// 딜레이 주기? 
	//	UE_LOG(LogTemp, Warning, TEXT("BTService: Entity is casting a skill, skipping update"));

	//}

	if (Entity && Entity->EntitySkillComponent)
	{
		bool bNormalAvailable = Entity->EntitySkillComponent->bCanUseNormalSkill;
		bool bSpecialAvailable = Entity->EntitySkillComponent->bCanUseSpecialSkill;

		BlackboardComp->SetValueAsBool(BBKEY_BASKILLAVAILABLE, bNormalAvailable);
		BlackboardComp->SetValueAsBool(BBKEY_BBSKILLAVAILABLE, bSpecialAvailable);
	}

	// SelectedSkillID 키 설정: 
	// A스킬 조건이 만족되면 "Skill_Bite", 
	// B스킬 조건 만족시 "Skill_Charge", 
	// 둘 다 아니면 해당 키를 클리어
	if (bASkillCondition)
	{
		BlackboardComp->SetValueAsString(BBKEY_SELECTEDSKILLID, TEXT("Skill_Bite"));
	}
	else if (bBSkillCondition)
	{
		BlackboardComp->SetValueAsString(BBKEY_SELECTEDSKILLID, TEXT("Skill_Charge"));
	}
	else
	{
		BlackboardComp->ClearValue(BBKEY_SELECTEDSKILLID);
	}


	// 스킬 쿨타임이 구현 되어있지 않은 관계로 Range 비교 결과로만 스킬 시전 
	// 조건 계산:
	// A스킬 조건: Distance <= ARange AND AAvailable true
	// B스킬 조건: Distance <= BRange AND BAvailable true
	/*bool bASkillCondition = (Distance <= A_SkillRange) && bA_SkillAvailable;
	bool bBSkillCondition = (Distance <= B_SkillRange) && bB_SkillAvailable;*/

	/*if (DistanceToPlayer <= A_SkillRange)
	{
		bASkillCondition = true;

		BlackboardComp->SetValueAsBool(BBKEY_BASKILLCONDITION, bASkillCondition);
		BlackboardComp->SetValueAsBool(BBKEY_BBSKILLCONDITION, bBSkillCondition);
	}

	if (!bASkillCondition && DistanceToPlayer <= B_SkillRange)
	{
		bBSkillCondition = true;

		BlackboardComp->SetValueAsBool(BBKEY_BASKILLCONDITION, bASkillCondition);
		BlackboardComp->SetValueAsBool(BBKEY_BBSKILLCONDITION, bBSkillCondition);
	}*/
	//bool bASkillCondition = (DistanceToPlayer <= A_SkillRange);
	//bool bBSkillCondition = (DistanceToPlayer <= B_SkillRange);

	//BlackboardComp->SetValueAsBool(BBKEY_BASKILLCONDITION, bASkillCondition);
	//BlackboardComp->SetValueAsBool(BBKEY_BBSKILLCONDITION, bBSkillCondition);

	//// AttackType에 따라 최소 거리 값 설정
	//uint8 AttackType = BlackboardComp->GetValueAsInt(BBKEY_ATTACKTYPE);

	//// 최소 거리
	//float MinDistance = BlackboardComp->GetValueAsFloat(BBKEY_MINDISTANCE);

	//// 실제 최소 거리 계산
	//float CalculatedMinDistance = MinDistance + PlayerCapsuleRadius + MonsterCapsuleRadius;

	//UWorld* World = ControllingPawn->GetWorld();

	// 최소 거리 원 시각화 (몬스터 기준)
	//DrawDebugSphere(
	//	World,
	//	ControllingPawn->GetActorLocation(),
	//	CalculatedMinDistance,
	//	8,
	//	FColor::Blue,
	//	false,
	//	0.5f
	//);

	//// PC와 몬스터 사이의 거리 시각화
	//DrawDebugLine(
	//	World,
	//	ControllingPawn->GetActorLocation(),
	//	PlayerLocation,
	//	(DistanceToPlayer >= CalculatedMinDistance) ? FColor::Green : FColor::Red,
	//	false,
	//	0.1f,
	//	0,
	//	3.0f
	//);

	//// 거리 값 시각화
	//FVector MidPoint = (ControllingPawn->GetActorLocation() + PlayerLocation) * 0.5f;
	//DrawDebugString(
	//	World,
	//	MidPoint,
	//	FString::Printf(TEXT("Distance: %.1f / Min: %.1f"), DistanceToPlayer, CalculatedMinDistance),
	//	nullptr,
	//	FColor::White,
	//	0.1f,
	//	false
	//);
}
