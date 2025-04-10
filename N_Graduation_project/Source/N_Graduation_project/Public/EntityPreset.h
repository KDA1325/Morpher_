// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float, NewHealth);
class UEntityWidget;

UCLASS()
class N_GRADUATION_PROJECT_API AEntityPreset : public ACharacter, public ICharacterAllInterface
{
	GENERATED_BODY()

public:
	// 델리게이트 선언
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnHealthChanged OnHealthChanged;

	// Sets default values for this character's properties
	AEntityPreset();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget")
	UEntityWidget* EntityWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UWidgetComponent* WidgetComp; //액터에 붙이는 컴포넌트(블루프린트 위젯으로 지정)

	// 블루프린트에서 할당하는 WidgetComponent (이 컴포넌트에 위젯 블루프린트가 지정되어 있어야 함)
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	//UWidgetComponent* WidgetComp;

	UPROPERTY(EditAnywhere)
	float CurrentHP;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	FString currentEntityGroupID;

	UPROPERTY(EditAnywhere)
	FString currentPreset;
	// 최대 체력와 이동 속도
	float MaxHp;
	int32 currentSpeed;

	UPROPERTY(EditAnywhere)
	EnumAttackType currentAttackType;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

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
	float GetHPRatio(); // 최대 HP로 나누기

	UFUNCTION(BlueprintCallable, Category = "Data")
	EnumAttackType GetAttackType();
// AI Section
protected:
	virtual float GetAIPatrolRadius() override;
	virtual float GetAIDetectRange() override;
	virtual float GetAIAttackRange() override;
	virtual float GetAITurnSpeed() override;

};
