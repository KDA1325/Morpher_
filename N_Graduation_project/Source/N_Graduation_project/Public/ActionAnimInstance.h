// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "ActionAnimInstance.generated.h"

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

private:
	/** 애니메이션 몽타주 변수 */
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* M_Slash;
};
