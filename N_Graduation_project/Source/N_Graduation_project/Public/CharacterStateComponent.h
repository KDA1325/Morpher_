// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterStateComponent.generated.h"

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Move UMETA(DisplayName = "Move"),
	Action UMETA(DisplayName = "Action"),
	Dash UMETA(DisplayName = "Dash"),
	Dead UMETA(DisplayName = "Dead")
};
class AN_Graduation_projectCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UCharacterStateComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCharacterStateComponent();

    ECharacterState CurrentState;

    // 상태 변경 함수
    void ChangeState(ECharacterState NewState);

    // 상태 변경 시 필요한 행동 제약 적용
    void ApplyActionRestrictions();

    // 현재 상태를 반환하는 함수
    ECharacterState GetCurrentState() const { return CurrentState; }

    bool isAction;
    bool isDash;

};
