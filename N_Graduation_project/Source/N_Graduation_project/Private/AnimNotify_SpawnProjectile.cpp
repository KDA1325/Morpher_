// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_SpawnProjectile.h"
#include "EntityPreset.h"

void UAnimNotify_SpawnProjectile::Notify(USkeletalMeshComponent * MeshComp,UAnimSequenceBase * Animation)
{
	if(!MeshComp) return;

	AEntityPreset* Entity = Cast<AEntityPreset>(MeshComp->GetOwner());
	if(Entity)
	{
		Entity->SpawnProjectile_ThrowRock();
		UE_LOG(LogTemp,Warning,TEXT("UAnimNotify_SpawnProjectile: Projectile Spawned"));
	}
}
