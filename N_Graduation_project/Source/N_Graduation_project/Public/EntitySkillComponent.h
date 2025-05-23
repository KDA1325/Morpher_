// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ABEntityData.h"
#include "SkillData.h"
#include "SkillEffectData.h"
#include "EntitySkillComponent.generated.h"

class AEntityPreset;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class N_GRADUATION_PROJECT_API UEntitySkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEntitySkillComponent();

	// Skill ID 값을 받아 스킬을 실행하는 함수 
	UFUNCTION(BlueprintCallable, Category = "Skill")
	void ExecuteSkill(const FString& SkillID);


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	// 쿨타임 플래그
	UPROPERTY(BlueprintReadOnly, Category = "Skill|Cooldown")
	bool bCanUseNormalSkill;

	UPROPERTY(BlueprintReadOnly, Category = "Skill|Cooldown")
	bool bCanUseSpecialSkill;


private:
	AEntityPreset* OwnerEntity;

	// Skill ID 값을 통해 SkillData와 SKillEffectData를 불러오는 함수
	bool LoadSkillDataBySkillID(const FString& SkillID, FSkillData& OutSkillData, TArray<FSkillEffectData>& OutEffectData);

	// SkillType: HitBox 인 스킬을 실행하는 함수 
	void ExecuteHitBoxTypeSkill(const FSkillData& SkillData, const TArray<FSkillEffectData>& EffectData);

	// SkillType: Projectile 인 스킬을 실행하는 함수 
	void ExecuteProjectileTypeSkill(const FSkillData& SkillData, const TArray<FSkillEffectData>& EffectData);

	// SkillType: Buff 인 스킬을 실행하는 함수 
	void ExecuteBuffTypeSkill(const FSkillData& SkillData, const TArray<FSkillEffectData>& EffectData);

	void ExecuteSkill_Charge(const FSkillData& SkillData, const TArray<FSkillEffectData>& EffectData);

	void ExecuteSkill_FireBall(const FSkillData& SkillData, const TArray<FSkillEffectData>& EffectData);

	void ExecuteSkill_FreezeBreath(const FSkillData & SkillData,const TArray<FSkillEffectData>& EffectData);

	void ExecuteSkill_EarthBreaker(const FSkillData & SkillData,const TArray<FSkillEffectData>& EffectData);

	void ExecuteSkill_SplinterArrow(const FSkillData& SkillData, const TArray<FSkillEffectData>& EffectData);

	/* 쿨타임 관리 함수 */
	void SetSkillTimer(float CooldownTime, FTimerDelegate TimerDelegate, bool bIsSpecial);
	void NormalCooldown(); // 일반 스킬 쿨타임 끝남
	void SpecialCooldown(); // 특수 스킬 쿨타임 끝남


protected:
	// 타이머 핸들 (일반, 특수 스킬 각각 필요 시 분리해서 관리)
	FTimerHandle NormalSkillTimerHandle;
	FTimerHandle SpecialSkillTimerHandle;
};
