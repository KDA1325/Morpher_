// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
#include "EngineMinimal.h" // USkeletalMeshComponent를 사용하기 위해 변경
#include "ABEntityData.h" // Entity Data 구조체
#include "ABGameSingleton.h"
#include "GameFramework/Character.h"
#include "Components/WidgetComponent.h"
#include "CharacterAllInterface.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimBlueprint.h"
#include "EntityPreset.generated.h"

UCLASS()
class N_GRADUATION_PROJECT_API AEntityPreset : public ACharacter, public ICharacterAllInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEntityPreset();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	FString currentEntityGroupID;

	UPROPERTY(EditAnywhere)
	FString currentPreset;

	UPROPERTY(EditAnywhere)
	float CurrentHP;	
	int32 currentSpeed;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// EntitySpawner 클래스를 참조하는 변수 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TSubclassOf<AActor> EntitySpawnerClass;

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void InitializeEntity(FABEntityData& InEntityData);

	// MaxHP를 설정하는 함수
	UFUNCTION(BlueprintCallable, Category = "Data")
	void SetHP(float NewHP);

	// MoveSpeed를 설정하는 함수
	UFUNCTION(BlueprintCallable, Category = "Data")
	void SetMoveSpeed(int32 MoveSpeed);

	void ApplyDamage(float DamageAmount);
	//void  WidgetUpdate();

	// AEntityCharacter.h
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

// AI Section
protected:
	virtual float GetAIPatrolRadius() override;
	virtual float GetAIDetectRange() override;
	virtual float GetAIAttackRange() override;
	virtual float GetAITurnSpeed() override;
};
