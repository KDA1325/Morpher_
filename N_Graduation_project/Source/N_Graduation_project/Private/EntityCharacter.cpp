// Fill out your copyright notice in the Description page of Project Settings.


#include "EntityCharacter.h"
#include "MyAIController.h"

// Sets default values
AEntityCharacter::AEntityCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AMyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AEntityCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	UpdateEntityData();
}

// Called every frame
void AEntityCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEntityCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEntityCharacter::GetEntityGroupID(FString EntityGroupID)
{
	EntityGroupID = m_entityGroupID;
}

void AEntityCharacter::UpdateEntityData()
{

}

void AEntityCharacter::SetMaxHp(int32 MaxHp)
{
}

void AEntityCharacter::SetMoveSpeed(int32 MoveSpeed)
{
}

void AEntityCharacter::SetPreset(FString PresetReference)
{
}
