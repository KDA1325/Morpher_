// Fill out your copyright notice in the Description page of Project Settings.

#include "EntitySkillComponent.h"
#include "EntityPreset.h"
#include "ABGameSingleton.h"

// Sets default values for this component's properties
UEntitySkillComponent::UEntitySkillComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	OwnerEntity = nullptr;
}

// Called when the game starts
void UEntitySkillComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	OwnerEntity = Cast<AEntityPreset>(GetOwner());
	if (!OwnerEntity)
	{
		UE_LOG(LogTemp, Error, TEXT("Owner entity is null"));
		return;
	}
	
}

// Called every frame
void UEntitySkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UEntitySkillComponent::ExecuteSkill(const FString& SkillID)
{
	FSkillData SkillData;
	TArray<FSkillEffectData> EffectDataArray;

	if (!LoadSkillDataBySkillID(SkillID, SkillData, EffectDataArray))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load skill data by skill id"));
		return;
	}

	// SkillType에 따라 스킬 실행(모듈화) 
	switch (SkillData.SkillType)
	{
	case EnumSkillType::HitBox:
		ExecuteHitBoxTypeSkill(SkillData, EffectDataArray);
		break;
	case EnumSkillType::Projectile:
		ExecuteProjectileTypeSkill(SkillData, EffectDataArray);
		break;
	case EnumSkillType::Buff:
		ExecuteBuffTypeSkill(SkillData, EffectDataArray);
		break;
	}
}

// Skill Data와 Skill Effect Data 불러옴
bool UEntitySkillComponent::LoadSkillDataBySkillID(const FString& SkillID, FSkillData& OutSkillData, TArray<FSkillEffectData>& OutEffectData)
{
	if (!UABGameSingleton::Get().GetSkillDataBySkillID(SkillID, OutSkillData))
	{
		UE_LOG(LogTemp, Error, TEXT("SkillData not found by %s"), *SkillID);

		return false;
	}

	if (!UABGameSingleton::Get().GetSkillEffectDataBySkillID(SkillID, OutEffectData))
	{
		UE_LOG(LogTemp, Error, TEXT("SkillEffectData not found by %s"), *SkillID);
	}

	return true;
}

// 애님 몽타주 노티파이로 호출 
// 특수 스킬(Special Skill)은 함수를 따로 만들어 이 안에서 호출 
void UEntitySkillComponent::ExecuteHitBoxTypeSkill(const FSkillData& SkillData, const TArray<FSkillEffectData>& EffectData)
{
	if (!OwnerEntity)
	{
		UE_LOG(LogTemp, Error, TEXT("Owner entity is null"));
		return;
	}

	if (SkillData.SkillNameID == "Skill_Charge")
	{
		//Entity->PerformSpecialSkill_Charge();
		UE_LOG(LogTemp, Warning, TEXT("Skill_Charge"));
	}
	else
	{
		OwnerEntity->ShowNormalHitBox();
	}
}

void UEntitySkillComponent::ExecuteProjectileTypeSkill(const FSkillData& SkillData, const TArray<FSkillEffectData>& EffectData)
{
	// 
}

void UEntitySkillComponent::ExecuteBuffTypeSkill(const FSkillData& SkillData, const TArray<FSkillEffectData>& EffectData)
{
	//
}
