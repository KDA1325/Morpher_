
#include "BossProjectile2.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"


ABossProjectile2::ABossProjectile2(){
	PrimaryActorTick.bCanEverTick = true;
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 300;
	ProjectileMovement->MaxSpeed = 300;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0;

}

// Called when the game starts or when spawned
void ABossProjectile2::BeginPlay()
{
	Super::BeginPlay();

}


void ABossProjectile2::InitProjectileBySkillData(float Speed,float damage)
{
	if(!CollisionComp)
	{
		CollisionComp = FindComponentByClass<USphereComponent>();
	}

	if(CollisionComp && ProjectileMovement)
	{
		//ProjectileMovement->ProjectileGravityScale = FMath::RandRange(0.03f,0.2f); //랜덤한 값을 줌

		//UE_LOG(LogTemp,Warning,TEXT("SpinningBarrage CollisionComp found: %s"),*CollisionComp->GetName());

		RootComponent = CollisionComp;

		ProjectileMovement->SetUpdatedComponent(CollisionComp);
		ProjectileMovement->Activate(true);

		//CollisionComp->OnComponentHit.AddDynamic(this,&AEntityProjectile::OnHit);
		CollisionComp->OnComponentBeginOverlap.AddDynamic(this,&ABossProjectile2::OnOverlap);
	} else
	{
		//	UE_LOG(LogTemp,Warning,TEXT("SpinningBarrage Hitbox is null"));
	}

	//	UE_LOG(LogTemp,Warning,TEXT("SpinningBarrage InitProjectileBySkillData 발동"));
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
		float LifeTime = 5;
		GetWorld()->GetTimerManager().SetTimer(
			DestroyTimerHandle,
			this,
			&ABossProjectile2::OnLifetimeExpired,
			LifeTime,
			false
		);
	}

}

void ABossProjectile2::OnOverlap(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult)
{
	if(!OtherActor) return;
	//UE_LOG(LogTemp,Warning,TEXT("SpinningBarrage 오버랩 발동"));

	// 플레이어만 적용
	if(OtherActor->ActorHasTag("Player"))
	{
		UGameplayStatics::ApplyDamage(OtherActor,25,GetInstigatorController(),this,nullptr);
		Destroy(); // 플레이어에 닿았으면 투사체 제거
	} else
	{
		// 인페르몽이나 다른 Pawn과는 무시
		return;
	}
}
void ABossProjectile2::OnLifetimeExpired()
{
	Destroy(); // 또는 ReturnToPool();
}
void ABossProjectile2::FireInDirection(const FVector& ShootDirection)
{
	if(ProjectileMovement && CollisionComp)
	{
		ProjectileMovement->StopMovementImmediately();
		ProjectileMovement->SetUpdatedComponent(CollisionComp);
		ProjectileMovement->SetVelocityInLocalSpace(ShootDirection * ProjectileMovement->InitialSpeed);
		ProjectileMovement->Activate(true);

		//UE_LOG(LogTemp,Warning,TEXT("ProjectileMovement->ProjectileGravityScale %f"),ProjectileMovement->ProjectileGravityScale);
	}
}