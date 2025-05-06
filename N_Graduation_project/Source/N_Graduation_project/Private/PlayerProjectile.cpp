#include "PlayerProjectile.h"
#include "ABGameSingleton.h"
#include "GameFramework/ProjectileMovementComponent.h" //발사체
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

APlayerProjectile::APlayerProjectile()
{
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

void APlayerProjectile::BeginPlay()
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

		CollisionComp->OnComponentBeginOverlap.AddDynamic(this,&APlayerProjectile::OnOverlap);
	} 
	else
	{
		UE_LOG(LogTemp,Error,TEXT("Hitbox is null in BeginPlay! Check BP binding."));
	}
}

void APlayerProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerProjectile::InitProjectileBySkillData(const FSkillData& InSkillData,const TArray<FSkillEffectData>& InEffectData)
{
	SkillData = InSkillData;
	EffectDataArray = InEffectData;

	if(ProjectileMovement)
	{
		ProjectileMovement->InitialSpeed = SkillData.ProjectileSpeed;
		ProjectileMovement->MaxSpeed = SkillData.ProjectileSpeed;
	}

	if(CollisionComp && GetOwner())
	{
		CollisionComp->IgnoreActorWhenMoving(GetOwner(),true);
	}
}

void APlayerProjectile::FireInDirection(const FVector& ShootDirection)
{
	if(ProjectileMovement && CollisionComp)
	{
		ProjectileMovement->StopMovementImmediately();
		ProjectileMovement->SetUpdatedComponent(CollisionComp);
		ProjectileMovement->SetVelocityInLocalSpace(ShootDirection * ProjectileMovement->InitialSpeed);
		ProjectileMovement->Activate(true);
		UE_LOG(LogTemp,Warning,TEXT("PPAP FireInDirection called! Velocity = %s"),*ProjectileMovement->Velocity.ToString());
	}
}

void APlayerProjectile::OnOverlap(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult)
{
	if(!OtherActor) return;

	// 몬스터만 처리
	if(OtherActor->ActorHasTag("Monster"))
	{
		for(const FSkillEffectData& Effect : EffectDataArray)
		{
			switch(Effect.EffectType)
			{
			case EnumEffectType::Damage:
			UGameplayStatics::ApplyDamage(OtherActor,Effect.EffectValue01,GetInstigatorController(),this,nullptr);
			break;
			//case EnumEffectType::AOEDamage:
	     	// 광역 대미지 처리
		    //break;
			case EnumEffectType::Fire:
			// 불 디버프 처리
			break;
			}
		}

		Destroy(); // 충돌 후 제거
	}
}
