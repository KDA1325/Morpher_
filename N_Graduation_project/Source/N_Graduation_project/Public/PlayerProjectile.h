#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkillData.h"
#include "SkillEffectData.h"
#include "PlayerProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class N_GRADUATION_PROJECT_API APlayerProjectile: public AActor
{
	GENERATED_BODY()

public:
	APlayerProjectile();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	void InitProjectileBySkillData(const FSkillData& InSkillData,const TArray<FSkillEffectData>& InEffectData);
	void FireInDirection(const FVector& ShootDirection);

	// Settings
	float Damage;
	float AOERadius;
	bool bApplyFireDot;
	float FireDamage;
	float FireDuration;

protected:
	UPROPERTY(VisibleAnywhere)
		USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere)
		UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY()
		FSkillData SkillData;

	UPROPERTY()
		TArray<FSkillEffectData> EffectDataArray;

	UFUNCTION()
		void OnOverlap(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);
};
