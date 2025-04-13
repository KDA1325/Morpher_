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

	bCanUseNormalSkill = true;
	bCanUseSpecialSkill = true;
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

	if (SkillID == "Skill_Charge")
	{
		if (!bCanUseSpecialSkill)
		{
			UE_LOG(LogTemp, Error, TEXT("skill is on cooldown"));
			return;
		}

		ExecuteSkill_Charge(SkillData, EffectDataArray);

		// 쿨타임 적용: 스킬 타입에 따라 타이머를 통해 재사용 가능 상태 복구
		bCanUseSpecialSkill = false;
		FTimerDelegate SpecialDelegate = FTimerDelegate::CreateUObject(this, &UEntitySkillComponent::SpecialCooldown);
		SetSkillTimer(SkillData.SkillCoolTime, SpecialDelegate);
	}
	else
	{
		if (!bCanUseNormalSkill)
		{
			UE_LOG(LogTemp, Warning, TEXT("Normal skill %s is on cooldown"), *SkillID);
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

		bCanUseNormalSkill = false;
		FTimerDelegate NormalDelegate = FTimerDelegate::CreateUObject(this, &UEntitySkillComponent::NormalCooldown);
		SetSkillTimer(SkillData.SkillCoolTime, NormalDelegate);
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

void UEntitySkillComponent::SetSkillTimer(float CooldownTime, FTimerDelegate TimerDelegate)
{
	if (CooldownTime > 0)
	{
		GetWorld()->GetTimerManager().SetTimer((CooldownTime == CooldownTime) ? NormalSkillTimerHandle : SpecialSkillTimerHandle, TimerDelegate, CooldownTime, false);
	}
}

void UEntitySkillComponent::NormalCooldown()
{
	bCanUseNormalSkill = true;

	UE_LOG(LogTemp, Log, TEXT("Normal skill cooldown ended"));
}

void UEntitySkillComponent::SpecialCooldown()
{
	bCanUseSpecialSkill = true;
	UE_LOG(LogTemp, Log, TEXT("Special skill cooldown ended"));
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

	OwnerEntity->ShowNormalHitBox();
}

void UEntitySkillComponent::ExecuteProjectileTypeSkill(const FSkillData& SkillData, const TArray<FSkillEffectData>& EffectData)
{
	// Projectile 스킬 구현 
}

void UEntitySkillComponent::ExecuteBuffTypeSkill(const FSkillData& SkillData, const TArray<FSkillEffectData>& EffectData)
{
	// Buff 스킬 구현 
}

void UEntitySkillComponent::ExecuteSkill_Charge(const FSkillData& SkillData, const TArray<FSkillEffectData>& EffectData)
{
	if (!OwnerEntity)
	{
		UE_LOG(LogTemp, Error, TEXT("Owner entity is null"));
		return;
	}

	OwnerEntity->PerformSkill_Charge();
}
