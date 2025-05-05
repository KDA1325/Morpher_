// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ABEntityData.h"
#include "SkillData.h"
#include "SkillEffectData.h"
#include "GameFramework/Actor.h"
#include "EntityProjectile.generated.h"

UCLASS()
class N_GRADUATION_PROJECT_API AEntityProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEntityProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Components
	//UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Components")
	//class USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Collision")
	class USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Components")
	class UProjectileMovementComponent* ProjectileMovement;

	// Settings
	float Damage;
	float AOERadius;
	bool bApplyFireDot;
	float FireDamage;
	float FireDuration;

	
	//void InitProjectile(const FSkillData& SkillData,const TArray<FSkillEffectData>& EffectData);

	// 투사체가 사용할 스킬 데이터
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Skill")
	FSkillData SkillData;

	// 투사체에 적용될 스킬 효과 데이터 (여러 개 가능)
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Skill")
	TArray<FSkillEffectData> EffectDataArray;

	// 스킬 데이터 기반 발사체 설정 
	void InitProjectileBySkillData(const FSkillData& InSkillData,const TArray<FSkillEffectData>& InEffectData);

	// 투사체가 충돌했을 때 호출 (스킬 효과 Damage, AOE, Fire 등 적용)
	UFUNCTION()
	//void OnHit(UPrimitiveComponent* HitComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,FVector NormalImpulse,const FHitResult& Hit);

	void OnOverlap(UPrimitiveComponent * OverlappedComp,AActor * OtherActor,UPrimitiveComponent * OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult & SweepResult);

	// ProjectileMovementComponent 사용을 위한 함수(투사체 발사) 
	void FireInDirection(const FVector & ShootDirection);

	//// Collision handling
	//UFUNCTION()
	//void OnProjectileHit(UPrimitiveComponent* HitComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,FVector NormalImpulse,const FHitResult& Hit);
};
