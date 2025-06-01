
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossProjectile.generated.h"


UCLASS()
class N_GRADUATION_PROJECT_API ABossProjectile : public AActor
{
	GENERATED_BODY()
public:
	ABossProjectile();

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Collision")
		class USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Components")
		class UProjectileMovementComponent* ProjectileMovement;

	void InitProjectileBySkillData(float Speed,float damage);
	void FireInDirection(const FVector& ShootDirection);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimerHandle DestroyTimerHandle;

	UFUNCTION()
		void OnOverlap(UPrimitiveComponent * OverlappedComp,AActor * OtherActor,UPrimitiveComponent * OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult & SweepResult);

	void OnLifetimeExpired();

	float DamageAmount;

};
