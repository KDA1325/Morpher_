// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_SpawnArrow.h"
#include "EntityPreset.h"
#include "N_Graduation_project/N_Graduation_projectCharacter.h"
#include "PlayerSkillComponent.h"
#include "EntityProjectile.h"

void UAnimNotify_SpawnArrow::Notify(USkeletalMeshComponent * MeshComp,UAnimSequenceBase * Animation)
{
	if(!MeshComp) return;

	if(AEntityPreset* Entity = Cast<AEntityPreset>(MeshComp->GetOwner()))
	{
		//Entity->SpawnProjectile_Arrow();
		UE_LOG(LogTemp,Warning,TEXT("UAnimNotify_SpawnArrow: Entity Arrow Spawned"));

		//// 소켓 위치 가져오기
		//FVector SpawnLocation = MeshComp->GetSocketLocation(TEXT("ArrowSocket"));
		//FRotator SpawnRotation = MeshComp->GetSocketRotation(TEXT("ArrowSocket"));

		//FActorSpawnParameters Params;
		//Params.Owner = Entity;

		//AEntityProjectile* Arrow = MeshComp->GetWorld()->SpawnActor<AEntityProjectile>(
		//	Entity->NormalProjectileClass,SpawnLocation,SpawnRotation,Params);

		//if(Arrow)
		//{
		//	// 손에 부착
		//	Arrow->AttachToComponent(MeshComp,FAttachmentTransformRules::SnapToTargetNotIncludingScale,TEXT("ArrowSocket"));

		//	// 일시 정지 상태로 준비
		//	Entity->PendingArrow = Arrow;  // 나중에 발사용으로 기억
		//	UE_LOG(LogTemp,Warning,TEXT("[SpawnArrow] Arrow spawned and attached"));
		//}
	} 
}
