#include "PlayerProjectile.h"
#include "ABGameSingleton.h"
#include "GameFramework/ProjectileMovementComponent.h" //발사체
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "N_Graduation_project/N_Graduation_projectCharacter.h"

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
	} else
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
	SkillRange = InSkillData.SkillRange;

	if(ProjectileMovement)
	{
		ProjectileMovement->InitialSpeed = SkillData.ProjectileSpeed;
		ProjectileMovement->MaxSpeed = SkillData.ProjectileSpeed;
	}

	if(CollisionComp && GetOwner())
	{
		CollisionComp->IgnoreActorWhenMoving(GetOwner(),true);
	}

	// SkillRange까지 도달 후 투사체 삭제를 위한 타이머 
	if(SkillData.ProjectileSpeed > 0.f)
	{
		float LifeTime = SkillData.SkillRange / SkillData.ProjectileSpeed;
		GetWorld()->GetTimerManager().SetTimer(
			DestroyTimerHandle,
			this,
			&APlayerProjectile::OnLifetimeExpired,
			LifeTime,
			false
		);
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
			case EnumEffectType::AOEDamage:
			{
				// 광역 대미지 처리, 몬스터용 로직엔 추가할 필요 없을 듯(플레이어 쪽에 추가하기)
				FVector Origin = GetActorLocation();
				float Radius = Effect.EffectValue02;
				TArray<AActor*> OverlappingActors;
				UGameplayStatics::GetAllActorsOfClass(GetWorld(),AN_Graduation_projectCharacter::StaticClass(),OverlappingActors);

				for(AActor* Actor : OverlappingActors)
				{
					if(Actor->ActorHasTag("Player"))
					{
						float Distance = FVector::Dist(Actor->GetActorLocation(),Origin);
						if(Distance <= Radius)
						{
							UGameplayStatics::ApplyDamage(Actor,Effect.EffectValue01,GetInstigatorController(),this,nullptr);
							UE_LOG(LogTemp,Warning,TEXT("AOE Damage to %s"),*Actor->GetName());
						}
					}
				}
				break;
			}
			case EnumEffectType::Fire:{
				float ApplyDuration = Effect.EffectValue01;
				float DPS = Effect.EffectValue02;
				ApplyFireDOT(OtherActor,DPS,ApplyDuration);
				break;
			}
			break;
			}
		}

		Destroy(); // 충돌 후 제거
	}
}

void APlayerProjectile::ApplyFireDOT(AActor* Target,float DamagePerSecond,float ApplyDuration)
{
	if(!Target || DamagePerSecond <= 0.f || ApplyDuration <= 0.f) return;

	int32 TickCount = FMath::FloorToInt(ApplyDuration);
	for(int32 i = 1; i <= TickCount; ++i)
	{
		FTimerHandle FireTickHandle;
		// [캡처](매개변수)->Return Type{ 구현 몸체 } 
		FTimerDelegate FireTickDelegate = FTimerDelegate::CreateLambda([=,this]()
		{
			UGameplayStatics::ApplyDamage(Target,DamagePerSecond,GetInstigatorController(),this,nullptr);
			UE_LOG(LogTemp,Warning,TEXT("ApplyFireDOT: %f damage to %s (tick %d)"),DamagePerSecond,*Target->GetName(),i);
		});
		GetWorld()->GetTimerManager().SetTimer(FireTickHandle,FireTickDelegate,i,false);
	}
}

void APlayerProjectile::OnLifetimeExpired()
{
	Destroy(); // 또는 ReturnToPool();
}
