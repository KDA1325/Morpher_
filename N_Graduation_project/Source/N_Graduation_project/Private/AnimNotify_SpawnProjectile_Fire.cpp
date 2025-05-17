// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_SpawnProjectile_Fire.h"
#include "EntityPreset.h"
#include "N_Graduation_project/N_Graduation_projectCharacter.h"
#include "PlayerSkillComponent.h"
#include "PlayerProjectile.h"
void UAnimNotify_SpawnProjectile_Fire::Notify(USkeletalMeshComponent * MeshComp,UAnimSequenceBase * Animation)
{
	if(!MeshComp) return;

	AEntityPreset* Entity = Cast<AEntityPreset>(MeshComp->GetOwner());
	if(Entity)
	{
		Entity->SpawnProjectile_FireBall();
		UE_LOG(LogTemp,Warning,TEXT("UAnimNotify_SpawnProjectile_Fire: Projectile Spawned"));
	} 
	else if(AN_Graduation_projectCharacter* Player = Cast<AN_Graduation_projectCharacter>(MeshComp->GetOwner()))
	{
		Player->PlayerSkillComponent->SpawnProjectile_FireBall();
		UE_LOG(LogTemp,Warning,TEXT("UAnimNotify_SpawnProjectile_Fire: Player Projectile Spawned"));
	}
}
