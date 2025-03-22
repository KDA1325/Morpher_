// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionAnimInstance.h"

UActionAnimInstance::UActionAnimInstance() 
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> Slash_Montage(TEXT("/Game/Animation/Sample/retarget_Stable_Sword_Outward_Slash_Anim_mixamo_com"));
	if (Slash_Montage.Succeeded())
	{
		M_Slash = Slash_Montage.Object;
		UE_LOG(LogTemp, Warning, TEXT("Slash_Montage.Succeeded"));
	}
	
}
void PlayAnimation(const FString& EffectID) 
{


}
