// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ABGameSingleton.h"
#include "SkillData.h"
//#include "SkillEffectDat.h"
#include "Components/BoxComponent.h"  // UBoxComponent
#include "Components/ArrowComponent.h"  // UArrowComponent

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
	
	void SkillType(const FString& SkillID);
	float distance;
	float GetDistanceTo(const AActor* OtherActor) const;
	FString   CurrentSkillID ;//나중에 그냥 커런트스킬아이디 = ㅇ이렇게해도 되게? 아..똑같나? 필요없으면 나중에 삭제

	UPROPERTY()
	class UBoxComponent* HitBox;

	UPROPERTY()
	class UArrowComponent* Arrow;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
private:	
	FTimerHandle TimerHandle; // 타이머

	void OffDefenseSkill(); // 방어 스킬 비활성화
	void HideHitBox(); //히트박스 비활성화
	bool bHitBoxInitialized;
};
