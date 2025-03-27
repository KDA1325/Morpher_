// Fill out your copyright notice in the Description page of Project Settings.

#include "MyMonsterStatComponent.h"
#include "Tester.h"
#include "EntityWidget.h"
#include "Components/WidgetComponent.h"

// Sets default values for this component's properties
UMyMonsterStatComponent::UMyMonsterStatComponent()
{
	CurrentHP = 0;
}

void UMyMonsterStatComponent::BeginPlay()
{
	Super::BeginPlay();
	UpdateWidget();
}

void UMyMonsterStatComponent::SetHP(int NewHP)
{
	// HP가 0 이하로 떨어지면 0으로 설정
	if (NewHP <= 0)
	{
		CurrentHP = 0;
		//OnHPIsZero.Broadcast();
	}
	else
	{
		CurrentHP = NewHP;
		//UE_LOG(LogTemp, Log, TEXT("== Monster CurrentHP: %d"), CurrentHP);
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("Current HP: %f"), CurrentHP));

//		MyEntityWidget->UpdateHealthBar(CurrentHP);

	}
}

void UMyMonsterStatComponent::SetMaxHp(int MaxHp)
{
	// NewMaxHP를 먼저 업데이트한 후, PastMaxHP를 이전 값으로 저장
	PastMaxHP = NewMaxHP;
	NewMaxHP = MaxHp;
}

void UMyMonsterStatComponent::ApplyDamage(float DamageAmount)
{
	SetHP(CurrentHP - DamageAmount);
}

void UMyMonsterStatComponent::UpdateWidget()
{
	// GetOwner()를 통해 현재 액터에서 위젯 찾기
	if (UWidgetComponent* WidgetComponent = GetOwner()->FindComponentByClass<UWidgetComponent>())
	{
		UE_LOG(LogTemp, Log, TEXT("banana find WidgetComponent"));
		if (UEntityWidget* MyEntityWidget = Cast<UEntityWidget>(WidgetComponent->GetWidget()))
		{
			UE_LOG(LogTemp, Log, TEXT("banana UpdateWidget"));

			MyEntityWidget->UpdateHealthBar(CurrentHP);
			MyEntityWidget->ReceiveEntityName(FText::FromString(MonsterName));
			MyEntityWidget->ReceiveEntitySpeed(currentSpeed);
		}
	}
	else {
		UE_LOG(LogTemp, Log, TEXT("banana isn't find WidgetComponent"));

	}
}
