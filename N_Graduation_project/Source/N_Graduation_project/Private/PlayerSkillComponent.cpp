// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSkillComponent.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"

// Sets default values for this component's properties
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

// Called when the game starts
void UPlayerSkillComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}
void UPlayerSkillComponent::OnDefenseSkill()
{
    if (IsDefending) return; // 중복X

    IsDefending = true;
    UE_LOG(LogTemp, Warning, TEXT("방어 스킬 활성화!"));

    // 3초 후 방어 해제
    FTimerDelegate DefenseEnd;
    DefenseEnd.BindUObject(this, &UPlayerSkillComponent::OffDefenseSkill);
    SetSkillTimer(3.0f, DefenseEnd);
}

void UPlayerSkillComponent::OffDefenseSkill()
{
    IsDefending = false;
    UE_LOG(LogTemp, Warning, TEXT("방어 스킬 종료"));
}

