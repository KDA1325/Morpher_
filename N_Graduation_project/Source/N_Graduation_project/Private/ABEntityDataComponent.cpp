// Fill out your copyright notice in the Description page of Project Settings.


#include "ABEntityDataComponent.h"

// Sets default values for this component's properties
UABEntityDataComponent::UABEntityDataComponent()
{
	MaxHP = 200.0f;
	CurrentHP = MaxHP;

	bWantsInitializeComponent = true; // InitializeComponent함수를 호출 할 수 있도록 하기 위해서
}

// Called when the game starts
void UABEntityDataComponent::BeginPlay()
{
	Super::BeginPlay();
	SetHP(MaxHP);
	
}

float UABEntityDataComponent::ApplyDamage(float InDamage)
{
	const float PrevHP = CurrentHP;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0, InDamage);

	SetHP(PrevHP - ActualDamage);
	if (CurrentHP <= KINDA_SMALL_NUMBER)
	{
		OnHPZero.Broadcast();
	}

	return ActualDamage;
}

void UABEntityDataComponent::SetHP(float NewHP)
{
	CurrentHP = FMath::Clamp<float>(NewHP, 0, MaxHP);
	OnHPChanged.Broadcast(CurrentHP);
}

float UABEntityDataComponent::GetHpPercentage() const
{   // 현재 HP 비율을 계산 (0~1 사이의 값)
	if (MaxHP == 0)
	{
		return 0.0f;
	}
	return CurrentHP / MaxHP;
}
void UABEntityDataComponent::TransformEntity(float NewMaxHP)
{   // 변신 후 새로운 MaxHP에 맞춰 현재 HP를 재설정

	// 현재 HP 비율을 계산
	float HPPercentage = GetHpPercentage(); 

	// 새로운 MaxHP에 맞춰서 바뀐 CurrentHP를 계산
	float TransformedCurrentHP = NewMaxHP * HPPercentage;

	// 바뀐 CurrentHP 값을 SetHP 함수로 적용
	SetHP(TransformedCurrentHP);

	// 새 MaxHP 값으로 업데이트
	MaxHP = NewMaxHP;
}


