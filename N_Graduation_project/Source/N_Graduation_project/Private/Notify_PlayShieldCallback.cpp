// Fill out your copyright notice in the Description page of Project Settings.

#include "Notify_PlayShieldCallback.h"

void UNotify_PlayShieldCallback::Notify(USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Animation)
{
	// Fill out your copyright notice in the Description page of Project Settings.
	if(UAnimInstance* AnimInstance = MeshComp->GetAnimInstance())
	{
		AnimInstance->Montage_Pause();
		UE_LOG(LogTemp,Log,TEXT("몽타주 멈춤"));
	}
}