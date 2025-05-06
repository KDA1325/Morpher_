// Fill out your copyright notice in the Description page of Project Settings.


#include "EntityProjectile.h"
#include "ABGameSingleton.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEntityProjectile::AEntityProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 발사체 이동 컴포넌트 설정
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 600.f;
	ProjectileMovement->MaxSpeed = 600.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	//ProjectileMovement->Activate();

	// 기타 설정
	Damage = 0.f;
	AOERadius = 0.f;
	bApplyFireDot = false;
	FireDamage = 0.f;
	FireDuration = 0.f;
}

// Called when the game starts or when spawned
void AEntityProjectile::BeginPlay()
{
	Super::BeginPlay();


	if(!CollisionComp)
	{
		CollisionComp = FindComponentByClass<USphereComponent>();
	}

	if(CollisionComp && ProjectileMovement)
	{
		UE_LOG(LogTemp,Warning,TEXT("CollisionComp found: %s"),*CollisionComp->GetName());

		RootComponent = CollisionComp;

		ProjectileMovement->SetUpdatedComponent(CollisionComp);
		ProjectileMovement->Activate(true);

		//CollisionComp->OnComponentHit.AddDynamic(this,&AEntityProjectile::OnHit);
		CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AEntityProjectile::OnOverlap);
	}
	else
	{
		UE_LOG(LogTemp,Error,TEXT("Hitbox is null in BeginPlay! Check BP binding."));
	}
}

// Called every frame
void AEntityProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEntityProjectile::InitProjectileBySkillData(const FSkillData & InSkillData, const TArray<FSkillEffectData>& InEffectData)
{
	SkillData = InSkillData;
	EffectDataArray = InEffectData;

	// 발사 속도 설정
	if(ProjectileMovement)
	{
		ProjectileMovement->InitialSpeed = SkillData.ProjectileSpeed;
		ProjectileMovement->MaxSpeed = SkillData.ProjectileSpeed;

		//// 충돌 무시 설정
		//if(GetOwner())
		//{
		//	ProjectileMovement->IgnoreActorWhenMoving(GetOwner(),true);
		//}
	}

	if(CollisionComp && GetOwner())
	{
		CollisionComp->IgnoreActorWhenMoving(GetOwner(),true);
	}
}

//void AEntityProjectile::OnHit(UPrimitiveComponent* HitComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,FVector NormalImpulse,const FHitResult& Hit)
//{
//	if(OtherActor && OtherActor->ActorHasTag("Player")) 
//	{
//		for(const FSkillEffectData& Effect : EffectDataArray)
//		{
//			switch(Effect.EffectType)
//			{
//			case EnumEffectType::Damage:
//			UGameplayStatics::ApplyDamage(OtherActor,Effect.EffectValue01,GetInstigatorController(),this,nullptr);
//			break;
//			//case EnumEffectType::AOEDamage:
//			//// 광역 대미지 처리
//			//break;
//			case EnumEffectType::Fire:
//			// 불 디버프 적용
//			break;
//			}
//
//			Destroy(); // 플레이어에 닿았으면 투사체 제거
//		}
//	}
//
//	
//}
void AEntityProjectile::OnOverlap(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult)
{
	if(!OtherActor) return;

	// 플레이어만 적용
	if(OtherActor->ActorHasTag("Player"))
	{
		// 충돌시, EffectDataArray를 순회해서
		for(const FSkillEffectData& Effect : EffectDataArray)
		{
			switch(Effect.EffectType)
			{
			case EnumEffectType::Damage:
			UGameplayStatics::ApplyDamage(OtherActor,Effect.EffectValue01,GetInstigatorController(),this,nullptr);
			break;
			//case EnumEffectType::AOEDamage:
			//// 광역 대미지 처리
			//break;
			case EnumEffectType::Fire:
			// 불 디버프 적용
			break;
			}
		}

		Destroy(); // 플레이어에 닿았으면 투사체 제거
	} else
	{
		// 인페르몽이나 다른 Pawn과는 무시
		return;
	}
}

//void AEntityProjectile::FireInDirection(const FVector& ShootDirection)
//{
//	if(ProjectileMovement)
//	{
//		UE_LOG(LogTemp,Warning,TEXT("FireInDirection called!"));
//		ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
//	}
//}
void AEntityProjectile::FireInDirection(const FVector& ShootDirection)
{
	if(ProjectileMovement && CollisionComp)
	{
		ProjectileMovement->StopMovementImmediately();
		ProjectileMovement->SetUpdatedComponent(CollisionComp);  // <-- 여기에 명시
		ProjectileMovement->SetVelocityInLocalSpace(ShootDirection * ProjectileMovement->InitialSpeed);
		ProjectileMovement->Activate(true);

		UE_LOG(LogTemp,Warning,TEXT("FireInDirection called! Velocity = %s"),*ProjectileMovement->Velocity.ToString());
	}
}
//void AEntityProjectile::FireInDirection(const FVector& ShootDirection)
//{
//	if(ProjectileMovement && CollisionComp)
//	{
//		// 발사 직전 일시적으로 Overlap Only로 설정
//		CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
//		CollisionComp->SetCollisionResponseToAllChannels(ECR_Overlap);
//
//		// 물리 초기화
//		ProjectileMovement->StopMovementImmediately();
//		ProjectileMovement->SetUpdatedComponent(CollisionComp);
//		ProjectileMovement->SetVelocityInLocalSpace(ShootDirection * ProjectileMovement->InitialSpeed);
//		ProjectileMovement->Activate(true);
//
//		UE_LOG(LogTemp,Warning,TEXT("FireInDirection called! Velocity = %s"),*ProjectileMovement->Velocity.ToString());
//
//		// 딜레이 후 원래 콜리전 Preset 복원 (타이머 사용)
//		FTimerHandle TimerHandle;
//		GetWorldTimerManager().SetTimer(TimerHandle,[this]()
//		{
//			if(CollisionComp)
//			{
//				CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
//				CollisionComp->SetCollisionResponseToAllChannels(ECR_Block);
//				CollisionComp->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap); // 필요 시 세부 조정
//			}
//		},0.05f,false); // 0.05초 후 복원
//	}
//}
