// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h" 
#include "FireFloor.generated.h"

UCLASS()
class N_GRADUATION_PROJECT_API AFireFloor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFireFloor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void ApplyFireDOT(AActor* Target,float DamagePerSecond,float ApplyDuration);

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,
		UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult & SweepResult);
	
	UPROPERTY(VisibleAnywhere)
		UBoxComponent* BoxComponent;

	UPROPERTY(VisibleAnywhere)
		UParticleSystemComponent* FireParticle;
	//UFUNCTION(BlueprintImplementableEvent)
		//void OFF();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	bool ActiveFire;
	void Off_Fire();
	void On_Fire();

};
