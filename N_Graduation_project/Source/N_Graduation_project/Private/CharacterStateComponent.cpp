#include "CharacterStateComponent.h"

UCharacterStateComponent::UCharacterStateComponent(){
CurrentState = ECharacterState::Idle;
}

void UCharacterStateComponent::ChangeState(ECharacterState NewState)
{
    if (CurrentState == NewState) return;

    UE_LOG(LogTemp, Warning, TEXT("ChangeState called! NewState: %d"), (uint8)NewState);

    CurrentState = NewState;
    UE_LOG(LogTemp, Error, TEXT("Current State changed to: %d"), (uint8)CurrentState);

    UpdateAnimation();
    ApplyActionRestrictions();
    UpdateAnimation();
    ApplyActionRestrictions();
}

void UCharacterStateComponent::UpdateAnimation()
{
    switch (CurrentState)
    {
    case ECharacterState::Idle:
        // Idle 애니메이션 로드
        break;   
    case ECharacterState::Dash:
        // Dash 애니메이션 로드
        break;
    case ECharacterState::Move:
        // Move 애니메이션 로드
        break;
    case ECharacterState::Action:
        // Action 애니메이션 로드
        break;
    case ECharacterState::Dead:
        // Dead 애니메이션 로드
        break;
    default:
        break;
    }
}

void UCharacterStateComponent::ApplyActionRestrictions()
{
    // 각 상태별 불가능한 행동 설정 (이동, 스킬 등)
    switch (CurrentState)
    {
    case ECharacterState::Idle:
        break;
    case ECharacterState::Action:
        // 이동 조작X, 스킬 조작X, 변신 조작X
        break;
    case ECharacterState::Dash:
        // 이동 조작X, 스킬 조작X, 변신 조작X
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
