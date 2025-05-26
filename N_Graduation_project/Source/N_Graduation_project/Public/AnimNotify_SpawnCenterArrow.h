// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_SpawnCenterArrow.generated.h"

/**
 * 
 */
UCLASS()
class N_GRADUATION_PROJECT_API UAnimNotify_SpawnCenterArrow : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Animation) override;
};