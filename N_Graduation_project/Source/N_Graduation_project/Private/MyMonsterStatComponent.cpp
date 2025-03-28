// Fill out your copyright notice in the Description page of Project Settings.

#include "MyMonsterStatComponent.h"

#include "EntityWidget.h"
#include "Components/WidgetComponent.h"

// Sets default values for this component's properties
//UMyMonsterStatComponent::UMyMonsterStatComponent()
//{
//	CurrentHP = 0;
//}
//
//void UMyMonsterStatComponent::BeginPlay()
//{
//	Super::BeginPlay();
//}
//
//void UMyMonsterStatComponent::SetHP(int NewHP)
//{
//	if (NewHP <= 0)
//	{
//		CurrentHP = 0;
//		//OnHPIsZero.Broadcast();
//	}
//	else
//	{
//		CurrentHP = NewHP;
//		//UE_LOG(LogTemp, Log, TEXT("== Monster CurrentHP: %d"), CurrentHP);
//		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("Current HP: %f"), CurrentHP));
//		UpdateWidget();
////		MyEntityWidget->UpdateHealthBar(CurrentHP);
//
//	}
//}
//
//void UMyMonsterStatComponent::SetMaxHp(int MaxHp)
//{
//	NewMaxHP = MaxHp;
//
//}
//
//void UMyMonsterStatComponent::ApplyDamage(float DamageAmount)
//{
//	SetHP(CurrentHP - DamageAmount);
//}
//
//void UMyMonsterStatComponent::UpdateWidget()
//{
//	//// GetOwner()를 통해 현재 액터에서 위젯 찾기
//	//if (UWidgetComponent* WidgetComponent = GetOwner()->FindComponentByClass<UWidgetComponent>())
//	//{
//	//	UE_LOG(LogTemp, Log, TEXT("banana find WidgetComponent"));
//	//	if (UEntityWidget* MyEntityWidget = Cast<UEntityWidget>(WidgetComponent->GetWidget()))
//	//	{
//	//		UE_LOG(LogTemp, Log, TEXT("banana UpdateWidget"));
//
//	//		MyEntityWidget->UpdateHealthBar(CurrentHP, CurrentHP);
//	//		MyEntityWidget->ReceiveEntityName(FText::FromString(MonsterName));
//	//		MyEntityWidget->ReceiveEntitySpeed(currentSpeed);
//	//	}
//	//}
//	//else {
//	//	UE_LOG(LogTemp, Log, TEXT("banana isn't find WidgetComponent"));
//	//}
//}
