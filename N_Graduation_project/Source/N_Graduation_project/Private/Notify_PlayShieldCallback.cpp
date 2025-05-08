// Fill out your copyright notice in the Description page of Project Settings.

#include "N_Graduation_project/N_Graduation_projectCharacter.h"
#include "Notify_PlayShieldCallback.h"
#include "PlayerSkillComponent.h"

void UNotify_PlayShieldCallback::Notify(USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Animation)
{
	// Fill out your copyright notice in the Description page of Project Settings.
	if(UAnimInstance* AnimInstance = MeshComp->GetAnimInstance())
	{
		AnimInstance->Montage_Pause();
		UE_LOG(LogTemp,Log,TEXT("몽타주 멈춤"));
	}
}