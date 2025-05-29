// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_FireAllArrows.h"
#include "EntityPreset.h"
#include "N_Graduation_project/N_Graduation_projectCharacter.h"
#include "PlayerSkillComponent.h"
void UAnimNotify_FireAllArrows::Notify(USkeletalMeshComponent * MeshComp,UAnimSequenceBase * Animation)
{
	if(!MeshComp) return;

	if(AEntityPreset* Entity = Cast<AEntityPreset>(MeshComp->GetOwner()))
	{
		Entity->Fire_AllArrows();
		UE_LOG(LogTemp,Warning,TEXT("UAnimNotify_FireAllArrows: Entity Arrow Spawned"));
	}
	else if(AN_Graduation_projectCharacter* Player = Cast<AN_Graduation_projectCharacter>(MeshComp->GetOwner()))
	{
		Player->PlayerSkillComponent->Fire_AllArrows();
	}
}


