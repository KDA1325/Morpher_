// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"

#include "Barrel.generated.h"

UCLASS()
class N_GRADUATION_PROJECT_API ABarrel: public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABarrel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void WorkBarrel(float DA);

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
		USphereComponent* SphereComponent;



	UPROPERTY(BlueprintReadWrite,Category = "Object")
		float ExplosionDelay;

	UPROPERTY(BlueprintReadWrite,Category = "Object")
		int EffectRadius;
	UPROPERTY(BlueprintReadWrite,Category = "Object")
		bool ApplyDamage;
	UPROPERTY(BlueprintReadWrite,Category = "Object")
	float DamageAmount;

	UFUNCTION(BlueprintImplementableEvent)
	void StartExplosion();

private:
	void OnExplosionOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);


	float ElapsedExplosionTime = 0.0f;
	bool bIsExploding = false;
};
