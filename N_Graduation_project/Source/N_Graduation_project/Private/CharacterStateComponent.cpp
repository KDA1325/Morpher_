#include "CharacterStateComponent.h"
#include <N_Graduation_project/N_Graduation_projectCharacter.h>
UCharacterStateComponent::UCharacterStateComponent(){
CurrentState = ECharacterState::Idle;
}

void UCharacterStateComponent::ChangeState(ECharacterState NewState)
{
    if (CurrentState == NewState) return;

    UE_LOG(LogTemp, Warning, TEXT("ChangeState called! NewState: %d"), (uint8)NewState);

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
            MyCharacter->StartAction();
            MyCharacter->EndAction();
            break;
        case ECharacterState::Dash:
            // 이동 조작X, 스킬 조작X, 변신 조작X
            MyCharacter->StartAction();
            MyCharacter->EndAction();
            break;
        case ECharacterState::Move:
            break;
        case ECharacterState::Dead:
            // 모든 행동
            MyCharacter->StartAction();

            break;
        default:
            break;
        }
    }
}
