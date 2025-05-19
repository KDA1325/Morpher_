// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_SpawnProjectile.h"
#include "EntityPreset.h"
#include "N_Graduation_project/N_Graduation_projectCharacter.h"
#include "PlayerSkillComponent.h"

void UAnimNotify_SpawnProjectile::Notify(USkeletalMeshComponent * MeshComp,UAnimSequenceBase * Animation)
{
	if(!MeshComp) return;

	if(AEntityPreset* Entity = Cast<AEntityPreset>(MeshComp->GetOwner()))
	{
		Entity->SpawnProjectile_ThrowRock();
		UE_LOG(LogTemp,Warning,TEXT("UAnimNotify_SpawnProjectile: Entity Projectile Spawned"));
	} else if(AN_Graduation_projectCharacter* Player = Cast<AN_Graduation_projectCharacter>(MeshComp->GetOwner()))
	{
		Player->PlayerSkillComponent->SpawnProjectile_ThrowRock();
		UE_LOG(LogTemp,Warning,TEXT("UAnimNotify_SpawnProjectile: Player Projectile Spawned"));
	}

}
