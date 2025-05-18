// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Totem_Projectile.generated.h"

UCLASS()
class N_GRADUATION_PROJECT_API ATotem_Projectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATotem_Projectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Collision")
		class USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Components")
		class UProjectileMovementComponent* ProjectileMovement;

	void InitProjectileBySkillData(float Speed,float damage);
	void FireInDirection(const FVector& ShootDirection);

protected:
	FTimerHandle DestroyTimerHandle;

	UFUNCTION()
		void OnOverlap(UPrimitiveComponent * OverlappedComp,AActor * OtherActor,UPrimitiveComponent * OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult & SweepResult);

	void OnLifetimeExpired();

	float DamageAmount;
};
