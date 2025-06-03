// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h" 
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "FrozeFloor.generated.h"

UCLASS()
class N_GRADUATION_PROJECT_API AFrozeFloor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFrozeFloor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,
		UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult & SweepResult);

	UPROPERTY(VisibleAnywhere)
		UBoxComponent* BoxComponent;

	UPROPERTY(VisibleAnywhere)
		UParticleSystemComponent* FreezeParticle;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
		UNiagaraComponent* FreezeNiagaraComponent;

	UPROPERTY(BlueprintReadWrite)
	float Duration ;

	UPROPERTY(BlueprintReadWrite)
	float SlowFactor;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	bool ActiveFroze;
	void Off_Froze();
	void On_Froze();

	TSet<AActor*> SlowedActors;
};
