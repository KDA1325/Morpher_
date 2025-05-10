// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Notify_PlayShieldCallback.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class N_GRADUATION_PROJECT_API UNotify_PlayShieldCallback : public UAnimNotify
{
	GENERATED_BODY()
public:
		virtual void Notify(USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Animation) override;
};
