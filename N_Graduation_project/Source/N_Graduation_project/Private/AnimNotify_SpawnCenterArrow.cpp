// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_SpawnCenterArrow.h"
#include "EntityPreset.h"

void UAnimNotify_SpawnCenterArrow::Notify(USkeletalMeshComponent * MeshComp,UAnimSequenceBase * Animation)
{
	if(!MeshComp) return;

	//if(AEntityPreset* Entity = Cast<AEntityPreset>(MeshComp->GetOwner()))
	//{
	//	Entity->Spawn_CenterArrow();
	//	UE_LOG(LogTemp,Warning,TEXT("UAnimNotify_SpawnCenterArrow: Entity Arrow Spawned"));
	//}
}
