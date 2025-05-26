// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_ProjectileArrow.h"
#include "EntityPreset.h"
#include "N_Graduation_project/N_Graduation_projectCharacter.h"
#include "PlayerSkillComponent.h"
#include "EntityProjectile.h"

void UAnimNotify_ProjectileArrow::Notify(USkeletalMeshComponent * MeshComp,UAnimSequenceBase * Animation)
{
	if(!MeshComp) return;

	if(AEntityPreset* Entity = Cast<AEntityPreset>(MeshComp->GetOwner()))
	{
		Entity->FireProjectile_Arrow();
		UE_LOG(LogTemp,Warning,TEXT("AnimNotify_ProjectileArrow: Entity Arrow Spawned"));

		//// 발사 방향 설정 
		//// 메시가 270도 회전된 상태가 X축 전방이기 때문에 Right Vector를 가져옴 
		//FVector Direction = MeshComp->GetRightVector();
		//Entity->PendingArrow->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		//Entity->PendingArrow->FireInDirection(Direction);  // 기존 FireInDirection 함수 사용
		//Entity->PendingArrow = nullptr;

		//UE_LOG(LogTemp,Warning,TEXT("[ProjectileArrow] Arrow fired"));
	}
}