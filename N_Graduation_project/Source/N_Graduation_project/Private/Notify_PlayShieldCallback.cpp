// Fill out your copyright notice in the Description page of Project Settings.

#include "Notify_PlayShieldCallback.h"
#include "N_Graduation_project/N_Graduation_projectCharacter.h"
#include "PlayerSkillComponent.h"

void UNotify_PlayShieldCallback::Notify(USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Animation)
{
	// Fill out your copyright notice in the Description page of Project Settings.
	if(UAnimInstance* AnimInstance = MeshComp->GetAnimInstance())
	{
		if(UAnimMontage* CurrentMontage = AnimInstance->GetCurrentActiveMontage())
		{
			AnimInstance->Montage_Pause(CurrentMontage);
			UE_LOG(LogTemp,Log,TEXT("몽타주 %s"),*CurrentMontage->GetName());
		} 
		else
		{
			UE_LOG(LogTemp,Warning,TEXT("현재 활성 몽타주가 없습니다."));
		}
	}
}