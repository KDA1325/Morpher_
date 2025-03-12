// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerSkillComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class N_GRADUATION_PROJECT_API UPlayerSkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerSkillComponent();

	void OnDefenseSkill(float Count); // 방어 스킬 활성화 함수
	void SetSkillTimer(float Count, FTimerDelegate Call); 	// 재사용가능하게
	bool IsDefending; // 방어중?
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:	
	FTimerHandle TimerHandle; // 타이머

	void OffDefenseSkill(); // 방어 스킬 비활성화
		
};
