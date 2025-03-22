// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ActionAnimInstance.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class N_GRADUATION_PROJECT_API UActionAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UActionAnimInstance();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> SlashMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Montage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* M_Slash;

	void PlayAnimation(const FString& EffectID);
};
