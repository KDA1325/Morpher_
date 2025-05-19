// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "ActionAnimInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMontageEndDelegate, UAnimMontage*, Montage, bool, bInterrupted);
/**
 * 
 */
UCLASS()
class N_GRADUATION_PROJECT_API UActionAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UActionAnimInstance();

	UFUNCTION(BlueprintCallable)
	void PlayAnimation(const FString& EffectID);

	// 애니메이션 종료 콜백 함수
	UFUNCTION()
	void OnMontageEndCallback(UAnimMontage* Montage, bool bInterrupted);
	void End_Shiled();
private:
	UAnimMontage* M_Slash;
	UAnimMontage* M_Bite;
	UAnimMontage* m_Inpermon1;
	UAnimMontage* m_Inpermon2;
	UAnimMontage* m_SkeletonSlash;
	UAnimMontage* m_Gaurd;
	UAnimMontage* m_Freezard1;
	UAnimMontage* m_Freezard2;

};
