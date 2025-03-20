// Fill out your copyright notice in the Description page of Project Settings.

// 스킬 사용 조건 플로우 처리
// 스킬 사용 가능 여부를 매 Tick 마다 확인
// A/B 스킬의 우선순위 판단, 스킬 범위 체크
// 스킬 사용 가능 시, 블랙보드에 스킬 타입과 타겟 설정
#include "BTService_SkillCheck.h"
#include "MyAI.h"
#include "MyAIController.h"
#include "CharacterAllInterface.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_SkillCheck::UBTService_SkillCheck()
{
	NodeName = TEXT("Skill Check");

	// 0.1초 단위로 수행 
	Interval = 1.0f;
}

void UBTService_SkillCheck::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// 지금 제어하고 있는 폰 정보 가져옴
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn)
	{
		return;
	}

	// AI 인터페이스 형 변환
	ICharacterAllInterface* AIPawn = Cast<ICharacterAllInterface>(ControllingPawn);
	if (nullptr == AIPawn)
	{
		return;
	}

	// 플레이어 가져오기
	ACharacter* PlayerCharacter = Cast<ACharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKEY_TARGET));
	if (nullptr == PlayerCharacter)
	{
		return;
	}

	// ControllingPawn과 플레이어 사이의 거리 저장 
	// 플레이어 위치 벡터 저장 
	//FVector PlayerLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(BBKEY_PLAYERLOCATION);
	//float DistanceToPlayer = FVector::Dist(ControllingPawn->GetActorLocation(), PlayerLocation);
	float DistanceToPlayer = FVector::Dist(ControllingPawn->GetActorLocation(), PlayerCharacter->GetActorLocation());

	// NormalSkill과 SpecialSkill의 Skilldata - SkillRange값을 비교
	// SkillData 데이터 테이블에 있는 SkillRange 불러와서 A/B스킬 정의 함수 구현  
	// SkillRange값이 더 낮은 스킬을 A스킬, 높은 스킬을 B스킬 이라고 정의
	//int SkillRangeA;
	//int SkillRangeB;
	
	// 쿨타임 상태 판정
	//bool skillACooldown;
	//bool skillBCooldown;
	
	// A스킬 시전 거리 조건 우선 계산
	//bool canUseSkillA = !skillACooldown && DistanceToPlayer <= SkillRangeA;

	// B스킬 시전 거리 조건 계산
	//bool canUseSkillB = !skillACooldown && DistanceToPlayer <= SkillRangeB;

	//// 스킬 범위와 쿨타임 체크
	//float SkillRangeA = AIPawn->GetAISkillRangeA();
	//float SkillRangeB = AIPawn->GetAISkillRangeB();
	//bool bSkillACooldown = AIPawn->IsSkillACooldown();
	//bool bSkillBCooldown = AIPawn->IsSkillBCooldown();
	
	// 스킬 사용
	//if (canUseSkillA)
	//{
	//	//OwnerComp.GetBlackboardComponent()->SetValueAsEnum(BBKEY_SKILLTYPE, (uint8)ESkillType::SkillA);
	//}

	//if (canUseSkillB)
	//{
	//	//OwnerComp.GetBlackboardComponent()->SetValueAsEnum(BBKEY_SKILLTYPE, (uint8)ESkillType::SkillB);

	//}

	/*if (canUseSkillA)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsEnum(BBKEY_SKILLTYPE, (uint8)ESkillType::SkillA);
	}
	else if (canUseSkillB)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsEnum(BBKEY_SKILLTYPE, (uint8)ESkillType::SkillB);
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->ClearValue(BBKEY_SKILLTYPE);
	}*/

	// AttackType에 따른 이동 플로우 적용
	// EntityData - AttackType이 Melee일 경우: 추적 플로우 진입
	// EntityData - AttackType이 Ranged일 경우: 거리 조절 플로우 진입 

}
