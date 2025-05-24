// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_UpdatePlayerLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "MyAI.h"
#include "AIController.h"
#include "ABGameSingleton.h"
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

	// PC-몬스터 거리 계산 및 저장 
    FVector AILocation = ControllingPawn->GetActorLocation();
    FVector PlayerLocation = playerPawn->GetActorLocation();
    float DistanceToPlayer = FVector::Dist(AILocation, PlayerLocation);

	BlackboardComp->SetValueAsVector(BBKEY_PLAYERLOCATION, PlayerLocation);
	BlackboardComp->SetValueAsFloat(BBKEY_DISTANCE, DistanceToPlayer);

	//------------------------------------------------------------------------------------------------------

	// 지금 제어하고 있는 폰의 EntityPreset 불러옴
	AEntityPreset* Entity = Cast<AEntityPreset>(ControllingPawn);
	if(!Entity || !Entity->EntitySkillComponent) return;

	// 지금 제어하고 있는 폰의 EntitySkillComponent 불러옴
	UEntitySkillComponent* SkillComponent = Entity->EntitySkillComponent;

	// 다른 데이터에 접근하기 위해 현재 제어하고 있는 폰의 EntityGropID 불러옴
	FString EntityGroupID = Entity -> currentPreset;

	// 각 EntityPreset에 저장된 스킬 ID 불러옴 
	FString NormalSkillID = Entity->NormalSkillID;
	FString SpecialSkillID = Entity->SpecialSkillID;

	// 싱글톤 Get 함수를 통해 불러올 스킬 관련 데이터를 저장할 변수
	float NormalSkillRange;
	float SpecialSkillRange;

	// 스킬 시전을 관리하기 위한 조건 플래그 
	bool bASkillCondition = false;
	bool bBSkillCondition = false;

	// SkillID를 키 값으로 스킬 관련 데이터 불러옴
	FSkillData SkillData;

	if(UABGameSingleton::Get().GetSkillDataBySkillID(NormalSkillID, SkillData))
	{
		NormalSkillRange = SkillData.SkillRange;
	}

	if(UABGameSingleton::Get().GetSkillDataBySkillID(SpecialSkillID, SkillData))
	{
		SpecialSkillRange = SkillData.SkillRange;
	}

	//------------------------------------------------------------------------------------------------------

	// 스킬 범위를 비교하여 AI에서 사용할 A/B 스킬 정의 
	FString A_SkillID;
	FString B_SkillID;
	float A_SkillRange;
	float B_SkillRange;

	if(NormalSkillRange < SpecialSkillRange)
	{
		A_SkillID = NormalSkillID;
		A_SkillRange = NormalSkillRange;

		B_SkillID = SpecialSkillID;
		B_SkillRange = SpecialSkillRange;
	} 
	else if(NormalSkillRange > SpecialSkillRange)
	{
		A_SkillID = SpecialSkillID;
		A_SkillRange = SpecialSkillRange;

		B_SkillID = NormalSkillID;
		B_SkillRange = NormalSkillRange;
	} 
	else // SkillRange가 같을 경우 Normal을 A로 우선시
	{
		A_SkillID = NormalSkillID;
		A_SkillRange = NormalSkillRange;

		B_SkillID = SpecialSkillID;
		B_SkillRange = SpecialSkillRange;
	}

	// 시전 가능 상태(쿨타임 돌아가고 있는 상태) 저장할 변수
	bool bCanUseA_Skill;
	bool bCanUseB_Skill;
	
	// A 스킬이 Normal 스킬이라면
	if(A_SkillID == NormalSkillID)
	{
		bCanUseA_Skill = SkillComponent->bCanUseNormalSkill;
		bCanUseB_Skill = SkillComponent->bCanUseSpecialSkill;
	}
	else // A 스킬이 Special 스킬이라면
	{
		bCanUseA_Skill = SkillComponent->bCanUseSpecialSkill;
		bCanUseB_Skill = SkillComponent->bCanUseNormalSkill;
	}

	// AI 로직에 의해 실행될 스킬 ID
	FString CastSkillID;

	// A 스킬 우선 평가
	if(bCanUseA_Skill && DistanceToPlayer <= A_SkillRange)
	{
		CastSkillID = A_SkillID;

		bASkillCondition = true;
		bBSkillCondition = false;
	} 
	else if(bCanUseB_Skill && DistanceToPlayer <= B_SkillRange)
	{
		CastSkillID = B_SkillID;

		bASkillCondition = false;
		bBSkillCondition = true;
	} 
	else
	{
		CastSkillID = nullptr; // 모든 스킬 사용 불가 상태

		bASkillCondition = false;
		bBSkillCondition = false;
	}
	
	BlackboardComp->SetValueAsString(BBKEY_CASTSKILLID, CastSkillID);

	BlackboardComp->SetValueAsBool(BBKEY_BASKILLCONDITION, bASkillCondition);
	BlackboardComp->SetValueAsBool(BBKEY_BBSKILLCONDITION, bBSkillCondition);
}

	//BlackboardComp->SetValueAsBool(TEXT("bASkillCondition"),bCanUseA_Skill && DistanceToPlayer <= A_SkillRange);
	//BlackboardComp->SetValueAsBool(TEXT("bBSkillCondition"),bCanUseB_Skill && DistanceToPlayer <= B_SkillRange);
	//BlackboardComp->SetValueAsString(TEXT("CastSkillID"),CastSkillID); // 선택된 스킬 ID는 여전히 보관


	//BlackboardComp->SetValueAsString(BBKEY_CASTSKILLID, CastSkillID);

	//// 블랙보드에 쿨타임 상태 저장
	//BlackboardComp->SetValueAsBool(BBKEY_BASKILLAVAILABLE, bNormalAvailable);
	//BlackboardComp->SetValueAsBool(BBKEY_BBSKILLAVAILABLE, bSpecialAvailable);

	//FString A_SkillID,B_SkillID;
	//FSkillData A_SkillData,B_SkillData;

	//// SkillRange 비교 기준으로 A/B 판별
	//if(NormalSkillData.SkillRange < SpecialSkillData.SkillRange)
	//{
	//	A_SkillID = NormalSkillID;
	//	B_SkillID = SpecialSkillID;
	//	A_SkillData = NormalSkillData;
	//	B_SkillData = SpecialSkillData;
	//}
	//else if(NormalSkillData.SkillRange > SpecialSkillData.SkillRange)
	//{
	//	A_SkillID = SpecialSkillID;
	//	B_SkillID = NormalSkillID;
	//	A_SkillData = SpecialSkillData;
	//	B_SkillData = NormalSkillData;
	//} else
	//{
	//	// 같을 경우 Normal을 A로 우선시
	//	A_SkillID = NormalSkillID;
	//	B_SkillID = SpecialSkillID;
	//	A_SkillData = NormalSkillData;
	//	B_SkillData = SpecialSkillData;
	//}

	//bool bA_Available = (A_SkillID == NormalSkillID) ? bNormalAvailable : bSpecialAvailable;
	//bool bB_Available = (B_SkillID == NormalSkillID) ? bNormalAvailable : bSpecialAvailable;

	//bool bASkillCondition = (DistanceToPlayer <= A_SkillData.SkillRange) && bA_Available;
	//bool bBSkillCondition = ((DistanceToPlayer > A_SkillData.SkillRange) && (DistanceToPlayer <= B_SkillData.SkillRange)) && bB_Available;

	//if(!UABGameSingleton::Get().GetSkillDataBySkillID(NormalSkillID,NormalSkillData) || !UABGameSingleton::Get().GetSkillDataBySkillID(SpecialSkillID,SpecialSkillData))
	//{
	//	return;
	//}

	//float A_SkillRange = NormalSkillData.SkillRange;
	//float B_SkillRange = SpecialSkillData.SkillRange;



	// A / B 스킬 정의
	//FSkillData* ASkill = nullptr;
	//FSkillData* BSkill = nullptr;
	//bool bAFromNormal = true;

	//if(NormalSkillData.SkillRange <= SpecialSkillData.SkillRange)
	//{
	//	ASkill = &NormalSkillData;
	//	BSkill = &SpecialSkillData;
	//	bAFromNormal = true;
	//} else
	//{
	//	ASkill = &SpecialSkillData;
	//	BSkill = &NormalSkillData;
	//	bAFromNormal = false;
	//}

	//// 거리 조건 판단
	//bool bASkillCondition = DistanceToPlayer <= ASkill->SkillRange && ((bAFromNormal && bNormalAvailable) || (!bAFromNormal && bSpecialAvailable));

	//bool bBSkillCondition = DistanceToPlayer <= BSkill->SkillRange && ((bAFromNormal && bSpecialAvailable) || (!bAFromNormal && bNormalAvailable));

	//BlackboardComp->SetValueAsFloat(BBKEY_ASKILLRANGE, ASkill->SkillRange);
	//BlackboardComp->SetValueAsFloat(BBKEY_BSKILLRANGE, BSkill->SkillRange);
	//BlackboardComp->SetValueAsBool(BBKEY_BASKILLCONDITION, bASkillCondition);
	//BlackboardComp->SetValueAsBool(BBKEY_BBSKILLCONDITION, bBSkillCondition);

	//// 선택 스킬 ID 저장
	//if(bASkillCondition)
	//{
	//	BlackboardComp->SetValueAsString(BBKEY_SELECTEDSKILLID,ASkill->SkillNameID);
	//} else if(bBSkillCondition)
	//{
	//	BlackboardComp->SetValueAsString(BBKEY_SELECTEDSKILLID,BSkill->SkillNameID);
	//} else
	//{
	//	BlackboardComp->ClearValue(BBKEY_SELECTEDSKILLID);
	//}

	//UE_LOG(LogTemp,Log,TEXT("Selected SkillID: %s"),*BlackboardComp->GetValueAsString(BBKEY_SELECTEDSKILLID));




	//// Blackboard로부터 값 읽어오기
	//float A_SkillRange = BlackboardComp->GetValueAsFloat(BBKEY_ASKILLRANGE);
	//float B_SkillRange = BlackboardComp->GetValueAsFloat(BBKEY_BSKILLRANGE);

	//bool bA_SkillAvailable = BlackboardComp->GetValueAsBool(BBKEY_BASKILLAVAILABLE);
	//bool bB_SkillAvailable = BlackboardComp->GetValueAsBool(BBKEY_BBSKILLAVAILABLE);

	//// 거리 조건 계산: 조건을 매 틱마다 새로 설정
	//bool bASkillCondition = (DistanceToPlayer <= A_SkillRange) && bA_SkillAvailable;
	//bool bBSkillCondition = ((DistanceToPlayer > A_SkillRange) && (DistanceToPlayer <= B_SkillRange)) && bB_SkillAvailable;

	//// 블랙보드에 조건 결과 업데이트
	//BlackboardComp->SetValueAsBool(BBKEY_BASKILLCONDITION, bASkillCondition);
	//BlackboardComp->SetValueAsBool(BBKEY_BBSKILLCONDITION, bBSkillCondition);

	//UE_LOG(LogTemp, Warning, TEXT("bASkillCondition: %s"), bASkillCondition ? TEXT("True") : TEXT("False"));
	//UE_LOG(LogTemp, Warning, TEXT("bBSkillCondition: %s"), bBSkillCondition ? TEXT("True") : TEXT("False"));
	//
	//AEntityPreset* Entity = Cast<AEntityPreset>(ControllingPawn);

	//// 만약 현재 엔티티가 스킬 시전 중이라면, SelectedSkillID 등 업데이트를 건너뛰고 리턴
	////if (!Entity->bIsCastingSkill)
	////{
	////	//// 거리 기반 스킬 조건 계산 및 SelectedSkillID 설정
	////	//BlackboardComp->SetValueAsBool(BBKEY_BASKILLCONDITION, bASkillCondition);
	////	//BlackboardComp->SetValueAsBool(BBKEY_BBSKILLCONDITION, bBSkillCondition);

	////	//if (bASkillCondition)
	////	//	BlackboardComp->SetValueAsString(BBKEY_SELECTEDSKILLID, TEXT("Skill_Bite"));
	////	//else if (bBSkillCondition)
	////	//	BlackboardComp->SetValueAsString(BBKEY_SELECTEDSKILLID, TEXT("Skill_Charge"));
	////	//else
	////	//	BlackboardComp->ClearValue(BBKEY_SELECTEDSKILLID);
	////	
	////	// 딜레이 주기? 
	////	UE_LOG(LogTemp, Warning, TEXT("BTService: Entity is casting a skill, skipping update"));

	////}

	//if (Entity && Entity->EntitySkillComponent)
	//{
	//	bool bNormalAvailable = Entity->EntitySkillComponent->bCanUseNormalSkill;
	//	bool bSpecialAvailable = Entity->EntitySkillComponent->bCanUseSpecialSkill;

	//	BlackboardComp->SetValueAsBool(BBKEY_BASKILLAVAILABLE, bNormalAvailable);
	//	BlackboardComp->SetValueAsBool(BBKEY_BBSKILLAVAILABLE, bSpecialAvailable);
	//}

	//// SelectedSkillID 키 설정: 
	//// A스킬 조건이 만족되면 "Skill_Bite", 
	//// B스킬 조건 만족시 "Skill_Charge", 
	//// 둘 다 아니면 해당 키를 클리어
	//if (bASkillCondition)
	//{
	//	BlackboardComp->SetValueAsString(BBKEY_SELECTEDSKILLID, TEXT("Skill_Bite"));
	//}
	//else if (bBSkillCondition)
	//{
	//	BlackboardComp->SetValueAsString(BBKEY_SELECTEDSKILLID, TEXT("Skill_Charge"));
	//}
	//else
	//{
	//	BlackboardComp->ClearValue(BBKEY_SELECTEDSKILLID);
	//}


	//// 스킬 쿨타임이 구현 되어있지 않은 관계로 Range 비교 결과로만 스킬 시전 
	//// 조건 계산:
	//// A스킬 조건: Distance <= ARange AND AAvailable true
	//// B스킬 조건: Distance <= BRange AND BAvailable true
	///*bool bASkillCondition = (Distance <= A_SkillRange) && bA_SkillAvailable;
	//bool bBSkillCondition = (Distance <= B_SkillRange) && bB_SkillAvailable;*/

	///*if (DistanceToPlayer <= A_SkillRange)
	//{
	//	bASkillCondition = true;

	//	BlackboardComp->SetValueAsBool(BBKEY_BASKILLCONDITION, bASkillCondition);
	//	BlackboardComp->SetValueAsBool(BBKEY_BBSKILLCONDITION, bBSkillCondition);
	//}

	//if (!bASkillCondition && DistanceToPlayer <= B_SkillRange)
	//{
	//	bBSkillCondition = true;

	//	BlackboardComp->SetValueAsBool(BBKEY_BASKILLCONDITION, bASkillCondition);
	//	BlackboardComp->SetValueAsBool(BBKEY_BBSKILLCONDITION, bBSkillCondition);
	//}*/
	////bool bASkillCondition = (DistanceToPlayer <= A_SkillRange);
	////bool bBSkillCondition = (DistanceToPlayer <= B_SkillRange);

	////BlackboardComp->SetValueAsBool(BBKEY_BASKILLCONDITION, bASkillCondition);
	////BlackboardComp->SetValueAsBool(BBKEY_BBSKILLCONDITION, bBSkillCondition);

	////// AttackType에 따라 최소 거리 값 설정
	////uint8 AttackType = BlackboardComp->GetValueAsInt(BBKEY_ATTACKTYPE);

	////// 최소 거리
	////float MinDistance = BlackboardComp->GetValueAsFloat(BBKEY_MINDISTANCE);

	////// 실제 최소 거리 계산
	////float CalculatedMinDistance = MinDistance + PlayerCapsuleRadius + MonsterCapsuleRadius;

	////UWorld* World = ControllingPawn->GetWorld();

	//// 최소 거리 원 시각화 (몬스터 기준)
	////DrawDebugSphere(
	////	World,
	////	ControllingPawn->GetActorLocation(),
	////	CalculatedMinDistance,
	////	8,
	////	FColor::Blue,
	////	false,
	////	0.5f
	////);

	////// PC와 몬스터 사이의 거리 시각화
	////DrawDebugLine(
	////	World,
	////	ControllingPawn->GetActorLocation(),
	////	PlayerLocation,
	////	(DistanceToPlayer >= CalculatedMinDistance) ? FColor::Green : FColor::Red,
	////	false,
	////	0.1f,
	////	0,
	////	3.0f
	////);

	////// 거리 값 시각화
	////FVector MidPoint = (ControllingPawn->GetActorLocation() + PlayerLocation) * 0.5f;
	////DrawDebugString(
	////	World,
	////	MidPoint,
	////	FString::Printf(TEXT("Distance: %.1f / Min: %.1f"), DistanceToPlayer, CalculatedMinDistance),
	////	nullptr,
	////	FColor::White,
	////	0.1f,
	////	false
	////);

