// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_ShowHitBox.generated.h"

/**
 * 
 */
UCLASS()
class N_GRADUATION_PROJECT_API UAnimNotify_ShowHitBox : public UAnimNotify
{
	GENERATED_BODY()

public:
	//UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Skill")
	//FString SkillID;

	//virtual FString GetNotifyName_Implementation() const override
	//{
	//	return FString::Printf(TEXT("ShowHitBox (%s)"),*SkillID);
	//}

	virtual void Notify(USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Animation) override;
};
