// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_FireAllArrows.h"
#include "EntityPreset.h"

void UAnimNotify_FireAllArrows::Notify(USkeletalMeshComponent * MeshComp,UAnimSequenceBase * Animation)
{
	if(!MeshComp) return;

	if(AEntityPreset* Entity = Cast<AEntityPreset>(MeshComp->GetOwner()))
	{
		Entity->Fire_AllArrows();
		UE_LOG(LogTemp,Warning,TEXT("UAnimNotify_FireAllArrows: Entity Arrow Spawned"));
	}
}


