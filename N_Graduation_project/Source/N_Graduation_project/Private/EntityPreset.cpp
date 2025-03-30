// Fill out your copyright notice in the Description page of Project Settings.


#include "EntityPreset.h"
#include "MyAIController.h"
#include "EntityWidget.h"

// Sets default values
AEntityPreset::AEntityPreset()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AMyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	CurrentHP = 0;
	currentSpeed = 0;
}

// Called when the game starts or when spawned
void AEntityPreset::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AEntityPreset::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEntityPreset::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AEntityPreset::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser); // 부모 클래스의 TakeDamage 호출
		UE_LOG(LogTemp, Log, TEXT("banana (TakeDamage)CurrentHP: %f, DamageAmount: %f, CurrentHP - DamageAmount: %f"), CurrentHP, DamageAmount, CurrentHP - DamageAmount);

	UE_LOG(LogTemp, Log, TEXT("banana TakeDamage called! Damage: %f"), DamageAmount);

	if (CurrentHP > 0)
	{
		ApplyDamage(DamageAmount);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("banana Entity Died!"));
		Destroy();
	}
	return ActualDamage;
}

void AEntityPreset::SetHP(float NewHP)
{
	CurrentHP = NewHP;
	UE_LOG(LogTemp, Warning, TEXT("SetHP NewHP: %f"), NewHP);
	/*if (NewHP <= 0)
	{
		CurrentHP = 0;
		UE_LOG(LogTemp, Log, TEXT("banana CurrentHP=0"));

	}
	else
	{
		CurrentHP = NewHP;
		UE_LOG(LogTemp, Log, TEXT("banana Monster CurrentHP: %f"), CurrentHP);
	}*/
}

void AEntityPreset::ApplyDamage(float DamageAmount)
{
	SetHP(CurrentHP - DamageAmount);

}

void AEntityPreset::SetMoveSpeed(int32 MoveSpeed)
{
	currentSpeed = MoveSpeed;
}

void AEntityPreset::InitializeEntity(FABEntityData& InEntityData)
{
	// Entity 데이터에 따라 초기화 
	SetActorLabel(InEntityData.EntityName);
	SetHP(InEntityData.HP);
	SetMoveSpeed(InEntityData.MoveSpeed);

	UE_LOG(LogTemp, Warning, TEXT("Initialized Entity with Name: %s, HP: %f, Move Speed: %d"),
		*InEntityData.EntityName, CurrentHP, InEntityData.MoveSpeed);
}

float AEntityPreset::GetAIPatrolRadius()
{
	return 800.0f; // 8미터
}

float AEntityPreset::GetAIDetectRange()
{
	return 900.0f; // 8미터
}

float AEntityPreset::GetAIAttackRange()
{
	return 0.0f;
}

float AEntityPreset::GetAITurnSpeed()
{
	return 0.0f;
}

//void AEntityPreset::WidgetUpdate()
//{
//	// EntityPresetClass에서 새로운 Actor를 스폰
//	AActor* SpawnedEntityPreset = GetWorld()->SpawnActor<AActor>(EntityPresetClass, GetActorLocation(), GetActorRotation());
//	// 스폰 실패 시 로그 출력
//	if (!SpawnedEntityPreset)
//	{
//		UE_LOG(LogTemp, Error, TEXT("banana SpawnEntityPreset failed!"));
//		return;
//	}
//
//	// 스폰된 Actor에서 WidgetComponent를 찾아서 UI 업데이트
//	UWidgetComponent* WidgetComponent = SpawnedEntityPreset->FindComponentByClass<UWidgetComponent>();
//
//	if (WidgetComponent)
//	{
//		UUserWidget* UserWidget = WidgetComponent->GetWidget();
//		if (UEntityWidget* MyEntityWidget = Cast<UEntityWidget>(UserWidget))
//		{
//			MyEntityWidget->UpdateHealthBar(CurrentHP);  // 체력 업데이트
//		}
//	}
//}