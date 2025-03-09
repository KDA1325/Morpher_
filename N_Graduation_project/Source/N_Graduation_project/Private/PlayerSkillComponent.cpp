#include "PlayerSkillComponent.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"

UPlayerSkillComponent::UPlayerSkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	IsDefending = false;
	
}

void UPlayerSkillComponent::SetSkillTimer(float Count, FTimerDelegate End)
{
    if (Count > 0)
    {
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, End, Count, false);
    }
}

void UPlayerSkillComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UPlayerSkillComponent::OnDefenseSkill()
{
    if (IsDefending) return; // 중복X

    IsDefending = true;
    
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("DefenseSkill on"));
    }
    // 3초 후 방어 해제
    FTimerDelegate DefenseEnd;
    DefenseEnd.BindUObject(this, &UPlayerSkillComponent::OffDefenseSkill); //3초 후 OffDefenseSkill호출
    SetSkillTimer(3.0f, DefenseEnd);
}

void UPlayerSkillComponent::OffDefenseSkill()
{
    IsDefending = false;
    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("DefenseSkill off")));
}

