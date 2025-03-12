#include "MyPlayerStatComponent.h"

UMyPlayerStatComponent::UMyPlayerStatComponent()
{
	// 기본 체력 설정
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
}

void UMyPlayerStatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMyPlayerStatComponent::TakeDamage(float DamageAmount)
{
    if (DamageAmount <= 0) return;

    CurrentHealth -= DamageAmount;

    // 체력 변화 출력
    UE_LOG(LogABGameSingleton, Error, TEXT("CurrentHealth: %f"), CurrentHealth);

    // 체력이 0 이하로 떨어지면 사망 처리
    if (CurrentHealth <= 0)
    {
        CurrentHealth = 0;
        OnDeath();  // 사망 이벤트 호출
    }

    // 체력 변화 이벤트 호출 (필요시 추가)
    // OnHealthChanged(CurrentHealth);
}


