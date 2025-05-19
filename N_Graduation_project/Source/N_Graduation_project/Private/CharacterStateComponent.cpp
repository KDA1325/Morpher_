//유한상태기계(FSM)
#include "CharacterStateComponent.h"
#include <N_Graduation_project/N_Graduation_projectCharacter.h>

UCharacterStateComponent::UCharacterStateComponent()
{
	CurrentState = ECharacterState::Idle;
}
// 캐릭터 Tcik에서 호출하기 -> ChangeState();
void UCharacterStateComponent::ChangeState(ECharacterState NewState)
{	// 상태 전환 시 초기화 작업 수행 가능
	 // Action 상태일 때는 다른 상태로 변경할 수 없도록
	if (CurrentState == ECharacterState::Action) 
	{
		if (isAction == true)
		{
			if (NewState == ECharacterState::Move || NewState == ECharacterState::Idle || NewState == ECharacterState::Dash)
			{
				return; // 이동 및 Idle 상태로 변경 안됨
			}
		}
	}

	if (CurrentState == ECharacterState::Dash)
	{
		if (isDash == true)
		{
			if (NewState == ECharacterState::Move || NewState == ECharacterState::Idle || NewState == ECharacterState::Action)
			{
				return; // 이동 및 Idle 상태로 변경 안됨
			}
		}
	}

	if (CurrentState == ECharacterState::Stun)
	{
		if (isStunned == true)
		{
			if (NewState == ECharacterState::Move || NewState == ECharacterState::Idle || NewState == ECharacterState::Action || NewState == ECharacterState::Dash)
			{
				return; // 위 상태로 변경 안됨
			}
		}
	}

	//if (CurrentState == ECharacterState::Dead)
	//{
	//	if (isAction == true)
	//	{
	//		if (NewState == ECharacterState::Move || NewState == ECharacterState::Idle || NewState == ECharacterState::Action || NewState == ECharacterState::Dash)
	//		{
	//			return; // 이동 및 Idle 상태로 변경 안됨
	//		}
	//	}
	//}

	if (CurrentState == NewState) return;

	CurrentState = NewState;
	UE_LOG(LogTemp, Error, TEXT("Current State changed to: %d"), (uint8)CurrentState);

	ApplyActionRestrictions();
}

void UCharacterStateComponent::ApplyActionRestrictions()
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()); // UCharacterStateComponent가 붙어 있는 캐릭터 인스턴스를 가져옴
	if (OwnerCharacter)
	{
		AN_Graduation_projectCharacter* MyCharacter = Cast<AN_Graduation_projectCharacter>(OwnerCharacter);
		// 특정 캐릭터 타입으로 캐스팅
		switch (CurrentState)
		{
		case ECharacterState::Idle:
			break;
		case ECharacterState::Action:
			// 이동 조작X, 스킬 조작X, 변신 조작X
			isAction = true;
			break;
		case ECharacterState::Dash:
			// 이동 조작X, 스킬 조작X, 변신 조작X
			break;
		case ECharacterState::Stun:
			// 이동, 스킬, 대시, 변신 조작X
			isStunned = true;
			break;
		case ECharacterState::Move:
			break;
		case ECharacterState::Dead:
			// 모든 행동
			break;
		default:
			break;
		}
	}
}