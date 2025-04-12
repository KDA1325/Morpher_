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
};
