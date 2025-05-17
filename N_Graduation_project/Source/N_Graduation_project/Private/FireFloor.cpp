// Fill out your copyright notice in the Description page of Project Settings.


#include "FireFloor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h" 

// Sets default values
AFireFloor::AFireFloor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BoxComponent = nullptr;
	ActiveFire=true;
}

// Called when the game starts or when spawned
void AFireFloor::BeginPlay()
{
	Super::BeginPlay();
	BoxComponent = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Box")));
	if(BoxComponent)
	{
		On_Fire();
		BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore); // 전부 무시하고
		BoxComponent->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap); // 원하는 채널만 오버랩
		BoxComponent->SetCollisionResponseToChannel(ECC_WorldDynamic,ECR_Overlap); // 필요시 추가

		BoxComponent->OnComponentBeginOverlap.AddDynamic(this,&AFireFloor::OnOverlapBegin);
		// 델리게이트 바인딩
		UE_LOG(LogTemp,Error,TEXT("Find BoxComponent"));

	} else
	{
		UE_LOG(LogTemp,Error,TEXT("BoxComponent가 NULL"));
	}

	FireParticle = Cast<UParticleSystemComponent>(GetDefaultSubobjectByName(TEXT("P_Fire")));

	if(FireParticle)
	{
		UE_LOG(LogTemp,Warning,TEXT("FireParticle 연결됨"));
	} else
	{
		UE_LOG(LogTemp,Error,TEXT("FireParticle 못 찾음"));
	}

}

// Called every frame
void AFireFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFireFloor::Off_Fire()
{
	ActiveFire=false;

	if(FireParticle){
		FireParticle->DeactivateSystem(); // 파티클 끄기
	}
}

void AFireFloor::On_Fire()
{
	ActiveFire=true;
	if(FireParticle){
		FireParticle->ActivateSystem(); // 파티클 켜기
	}
	UE_LOG(LogTemp,Warning,TEXT("On_Fire, ActiveFire %d"),ActiveFire);
}

void AFireFloor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,
	UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult & SweepResult)
{
	if(ActiveFire==true){
		if(!OtherActor->ActorHasTag("HitBox")){
			if(!ActiveFire || !OtherActor || OtherActor == this)
				return;
			if(OtherComp->ComponentHasTag("HitBox"))
			{
				UE_LOG(LogTemp,Warning,TEXT("On_Fire 무시됨: %s"),*OtherActor->GetName());
				return;
			}
			UE_LOG(LogTemp,Warning,TEXT("On_Fire 범위 내 감지된 액터: %s"),*OtherActor->GetName());
			ApplyFireDOT(OtherActor,10.0f,4.0f);
		}
	}
}

void AFireFloor::ApplyFireDOT(AActor* Target,float DamagePerSecond,float ApplyDuration)
{
	if(!Target || DamagePerSecond <= 0.f || ApplyDuration <= 0.f) return;
	if(Target){
		int32 TickCount = FMath::FloorToInt(ApplyDuration);
		for(int32 i = 1; i <= TickCount; ++i)
		{
			FTimerHandle FireTickHandle;
			FTimerDelegate FireTickDelegate = FTimerDelegate::CreateLambda([=,this]()
			{
				UGameplayStatics::ApplyDamage(Target,DamagePerSecond,GetInstigatorController(),this,nullptr);
				UE_LOG(LogTemp,Warning,TEXT("화상 횟수 %d"),i);
			});
			GetWorld()->GetTimerManager().SetTimer(FireTickHandle,FireTickDelegate,i,false);
		}
	}
}