// Fill out your copyright notice in the Description page of Project Settings.


#include "FireBarrel.h"
#include "Barrel.h"
#include "StunBarrel.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
// Sets default values
AFireBarrel::AFireBarrel()
{
	PrimaryActorTick.bCanEverTick = true;

	ExplosionDelay = 1.0f;
	ApplyDamage = false;
	SphereComponent = nullptr;
	DamageAmount = 0.0f;
}

// Called when the game starts or when spawned
void AFireBarrel::BeginPlay()
{
	Super::BeginPlay();
	SphereComponent = Cast<USphereComponent>(GetDefaultSubobjectByName(TEXT("Sphere")));

	if(SphereComponent)
	{
		SphereComponent->SetGenerateOverlapEvents(true);
		UE_LOG(LogTemp,Log,TEXT("Barrel: SphereComponent 세팅 완료"));
	} else
	{
		UE_LOG(LogTemp,Error,TEXT("Barrel: SphereComponent가 NULL입니다! BP에서 설정되었는지 확인하세요"));
	}
}

// Called every frame
void AFireBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// 외부에서 데미지 유입 시 호출
void AFireBarrel::WorkBarrel(float DA)
{
	GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Red,TEXT("Barrel: 응답받음"));
	DamageAmount = DA;

	StartExplosion();
}

// 폭발 후 실제 데미지 처리 및 연쇄 폭발 처리
void AFireBarrel::EndtExplosion()
{
	if(!SphereComponent)
	{
		UE_LOG(LogTemp,Error,TEXT("Barrel: SphereComponent가 없습니다"));
		return;
	}

	TArray<AActor*> OverlappingActors;
	SphereComponent->GetOverlappingActors(OverlappingActors);

	for(AActor* Actor : OverlappingActors)
	{
		if(!Actor || Actor == this)
			continue;

		UE_LOG(LogTemp,Warning,TEXT("폭발 범위 내 감지된 액터: %s"),*Actor->GetName());

		// Barrel인 경우 연쇄 폭발 처리
		if(Actor->ActorHasTag(FName("Barrel")))
		{
			if(AStunBarrel* StunBarrel = Cast<AStunBarrel>(Actor))
			{
				StunBarrel->WorkBarrel(DamageAmount); // AStunBarrel 고유 로직
			} else if(ABarrel* NormalBarrel = Cast<ABarrel>(Actor))
			{
				NormalBarrel->WorkBarrel(DamageAmount); // ABarrel 로직
			} else if(AFireBarrel* FireBarrel = Cast<AFireBarrel>(Actor)){
				FireBarrel->WorkBarrel(DamageAmount); // FireBarrel 로직

			}
		}

		// 데미지 처리
		AController* InstigatorController = GetInstigatorController();
		UGameplayStatics::ApplyDamage(Actor,DamageAmount,InstigatorController,this,nullptr);
	}
}
