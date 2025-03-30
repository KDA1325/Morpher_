// Fill out your copyright notice in the Description page of Project Settings.


#include "EntityPreset.h"
#include "MyAIController.h"
#include "EntityWidget.h"
#include "MyAI.h"
#include "BehaviorTree/BlackboardComponent.h"

// Sets default values
AEntityPreset::AEntityPreset()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AMyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	currentHp = 0;
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

void AEntityPreset::SetMaxHp(int32 MaxHp)
{
	currentHp = MaxHp;
}

void AEntityPreset::SetMoveSpeed(int32 MoveSpeed)
{
	currentSpeed = MoveSpeed;
}

void AEntityPreset::InitializeEntity(FABEntityData& InEntityData)
{
	// Entity 데이터에 따라 초기화 
	SetActorLabel(InEntityData.EntityName);
	SetMaxHp(InEntityData.HP);
	SetMoveSpeed(InEntityData.MoveSpeed);

	UE_LOG(LogTemp, Warning, TEXT("Initialized Entity with Name: %s, HP: %d, Move Speed: %d"),
		*InEntityData.EntityName, currentHp, currentSpeed);
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

EnumAttackType AEntityPreset::GetAttackType()
{
	UE_LOG(LogTemp, Warning, TEXT("Get AttackType: %d"), currentAttackType);

	return currentAttackType;
}
