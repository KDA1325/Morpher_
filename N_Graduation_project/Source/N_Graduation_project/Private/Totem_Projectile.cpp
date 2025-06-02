// Fill out your copyright notice in the Description page of Project Settings.


#include "Totem_Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ABGameSingleton.h"

// Sets default values
ATotem_Projectile::ATotem_Projectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 600.f;
	ProjectileMovement->MaxSpeed = 600.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	//ProjectileMovement->Activate();

}

// Called when the game starts or when spawned
void ATotem_Projectile::BeginPlay()
{
	Super::BeginPlay();

	if(!CollisionComp)
	{
		CollisionComp = FindComponentByClass<USphereComponent>();
	}

	if(CollisionComp && ProjectileMovement)
	{
		UE_LOG(LogTemp,Warning,TEXT("ATotem_Projectile CollisionComp found: %s"),*CollisionComp->GetName());

		RootComponent = CollisionComp;

		ProjectileMovement->SetUpdatedComponent(CollisionComp);
		ProjectileMovement->Activate(true);

		//CollisionComp->OnComponentHit.AddDynamic(this,&AEntityProjectile::OnHit);
		CollisionComp->OnComponentBeginOverlap.AddDynamic(this,&ATotem_Projectile::OnOverlap);
	} else
	{
		UE_LOG(LogTemp,Warning,TEXT("ATotem_Projectile Hitbox is null"));
	}

}

// Called every frame
void ATotem_Projectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATotem_Projectile::InitProjectileBySkillData(float Speed, float damage)
{
	UE_LOG(LogTemp,Warning,TEXT("ATotem_Projectile InitProjectileBySkillData 발동"));
	// 발사 속도 설정
	if(ProjectileMovement)
	{
		//ProjectileMovement->InitialSpeed = SkillData.ProjectileSpeed;
		//ProjectileMovement->MaxSpeed = SkillData.ProjectileSpeed;
		DamageAmount=damage;

		ProjectileMovement->InitialSpeed = Speed;
		ProjectileMovement->MaxSpeed = Speed;
	}
	if(CollisionComp && GetOwner())
	{
		CollisionComp->IgnoreActorWhenMoving(GetOwner(),true);
	}

	if(ProjectileMovement->InitialSpeed > 0.f)
	{
		//float LifeTime = 700/600;//SkillData.SkillRange / SkillData.ProjectileSpeed;
		float LifeTime = (600*5.f)/600;//SkillData.SkillRange / SkillData.ProjectileSpeed;
		GetWorld()->GetTimerManager().SetTimer(
			DestroyTimerHandle,
			this,
			&ATotem_Projectile::OnLifetimeExpired,
			LifeTime,
			false
		);
	}

}

void ATotem_Projectile::OnOverlap(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult)
{
	if(!OtherActor) return;
	UE_LOG(LogTemp,Warning,TEXT("ATotem_Projectile 오버랩 발동"));

	// 플레이어만 적용
	if(OtherActor->ActorHasTag("Player"))
	{
		UGameplayStatics::ApplyDamage(OtherActor,DamageAmount,GetInstigatorController(),this,nullptr);
		Destroy(); // 플레이어에 닿았으면 투사체 제거
	} 
	else
	{
		// 인페르몽이나 다른 Pawn과는 무시
		return;
	}
}
void ATotem_Projectile::OnLifetimeExpired()
{
	Destroy(); // 또는 ReturnToPool();
}
void ATotem_Projectile::FireInDirection(const FVector& ShootDirection)
{
	if(ProjectileMovement && CollisionComp)
	{
		ProjectileMovement->StopMovementImmediately();
		ProjectileMovement->SetUpdatedComponent(CollisionComp);  
		ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
		ProjectileMovement->Activate(true);

		UE_LOG(LogTemp,Warning,TEXT("ATotem_Projectile FireInDirection ~ called! Velocity = %s"),*ProjectileMovement->Velocity.ToString());
	}
}