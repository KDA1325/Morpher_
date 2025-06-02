#include "PlayerProjectile.h"
#include "ABGameSingleton.h"
#include "GameFramework/ProjectileMovementComponent.h" //발사체
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "N_Graduation_project/N_Graduation_projectCharacter.h"
#include "FireFloor.h"
#include "FrozeFloor.h"
#include "Barrel.h"
#include "StunBarrel.h"
#include "FireBarrel.h"

APlayerProjectile::APlayerProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->SetCollisionProfileName("Projectile");
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn,ECR_Block); // 캐릭터는 막음
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Block); // 타일, 벽, 바닥 등

	RootComponent = CollisionComp;
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
		//UE_LOG(LogTemp,Warning,TEXT("CollisionComp found: %s"),*CollisionComp->GetName());

		RootComponent = CollisionComp;

		ProjectileMovement->SetUpdatedComponent(CollisionComp);
		ProjectileMovement->Activate(true);

		CollisionComp->OnComponentBeginOverlap.AddDynamic(this,&APlayerProjectile::OnOverlap);
	} else
	{
		//UE_LOG(LogTemp,Error,TEXT("Hitbox is null in BeginPlay! Check BP binding."));
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

	//if(ProjectileMovement)
	//{
	//	ProjectileMovement->InitialSpeed = SkillData.ProjectileSpeed;
	//	ProjectileMovement->MaxSpeed = SkillData.ProjectileSpeed;
	//}
	UE_LOG(LogTemp,Warning,TEXT("InitProjectileBySkillData 실행됨"));

	if(CollisionComp && GetOwner())
	{
		CollisionComp->IgnoreActorWhenMoving(GetOwner(),true);
		UE_LOG(LogTemp,Warning,TEXT("CollisionComp 있음"));

	}


}

void APlayerProjectile::FireInDirection(const FVector& ShootDirection)
{
	if(ProjectileMovement && CollisionComp)
	{
		ProjectileMovement->StopMovementImmediately(); //먼저 멈추고

		ProjectileMovement->InitialSpeed = SkillData.ProjectileSpeed;
		ProjectileMovement->MaxSpeed = SkillData.ProjectileSpeed;

		ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
		UE_LOG(LogTemp,Warning,TEXT(">>> Projectile Velocity: %s"),*ProjectileMovement->Velocity.ToString());
		ProjectileMovement->SetUpdatedComponent(CollisionComp);
		ProjectileMovement->Activate(true);

		UE_LOG(LogTemp,Warning,TEXT("FireInDirection called! Velocity = %s"),*ProjectileMovement->Velocity.ToString());
	}
	UE_LOG(LogTemp,Warning,TEXT("ShootDirection: %s"),*ShootDirection.ToString());
	UE_LOG(LogTemp,Warning,TEXT("Set Velocity: %s"),*(ShootDirection * ProjectileMovement->InitialSpeed).ToString());

}

void APlayerProjectile::OnOverlap(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult)
{
	if(!OtherActor) return;
	if(OtherActor)
	{
		UE_LOG(LogTemp,Warning,TEXT("FireBall Overlapped with: %s"),*OtherActor->GetName());

		for(const FName& Tag : OtherActor->Tags)
		{
			UE_LOG(LogTemp,Warning,TEXT("Tag: %s"),*Tag.ToString());
		}
	}

	if(OtherActor->ActorHasTag("FireCrystal"))
	{
		UE_LOG(LogTemp,Warning,TEXT("FireCrystal activated!"));
		OtherActor->Destroy();
	}
	// 몬스터만 처리
	if(OtherActor->ActorHasTag("Monster")||OtherActor->ActorHasTag("Object")|| OtherActor->ActorHasTag("FireFloor")||OtherActor->ActorHasTag("FrozeFloor")||OtherActor->ActorHasTag("FireCrystal"))
	{
		UE_LOG(LogTemp,Warning,TEXT("OtherActor->ActorHasTag(Floor"));

		for(const FSkillEffectData& Effect : EffectDataArray)
		{
			switch(Effect.EffectType)
			{
			case EnumEffectType::Damage:
			UE_LOG(LogTemp,Warning,TEXT("데미지 실행됨"));

			UGameplayStatics::ApplyDamage(OtherActor,Effect.EffectValue01,GetInstigatorController(),this,nullptr);
			if(OtherActor->ActorHasTag(FName("Barrel")))
			{

				if(AStunBarrel* StunBarrel = Cast<AStunBarrel>(OtherActor))
				{
					StunBarrel->WorkBarrel(0); // AStunBarrel 고유 로직
				} else if(ABarrel* NormalBarrel = Cast<ABarrel>(OtherActor))
				{
					NormalBarrel->WorkBarrel(0); // ABarrel 로직
				} else if(AFireBarrel* FireBarrel = Cast<AFireBarrel>(OtherActor)){
					FireBarrel->WorkBarrel(0); // FireBarrel 로직

				}
			}
			break;
			case EnumEffectType::AOEDamage:
			{
				// 광역 대미지 처리, 몬스터용 로직엔 추가할 필요 없을 듯(플레이어 쪽에 추가하기)
				FVector Origin = GetActorLocation(); // AOE 중심
				float Radius = Effect.EffectValue02;
				Damage = Effect.EffectValue01;
				UGameplayStatics::ApplyDamage(OtherActor,Damage,GetInstigatorController(),this,nullptr);

				TArray<AActor*> OverlappingActors;
				UGameplayStatics::GetAllActorsOfClass(GetWorld(),AN_Graduation_projectCharacter::StaticClass(),OverlappingActors);

				if(OtherActor->ActorHasTag(FName("Barrel")))
				{
					if(AStunBarrel* StunBarrel = Cast<AStunBarrel>(OtherActor))
					{
						StunBarrel->WorkBarrel(0); // AStunBarrel 고유 로직
					} else if(ABarrel* NormalBarrel = Cast<ABarrel>(OtherActor))
					{
						NormalBarrel->WorkBarrel(0); // ABarrel 로직
					} else if(AFireBarrel* FireBarrel = Cast<AFireBarrel>(OtherActor)){
						FireBarrel->WorkBarrel(0); // FireBarrel 로직

					}
				}
				break;
			}
			case EnumEffectType::Fire:{
				float ApplyDuration = Effect.EffectValue01;
				float DPS = Effect.EffectValue02;
				ApplyFireDOT(OtherActor,DPS,ApplyDuration);
				UE_LOG(LogTemp,Warning,TEXT("FireFloor EnumEffectType::Fire!"));

				if(OtherActor->ActorHasTag(FName("FireFloor")))
				{
					if(AFireFloor* FireFloor = Cast<AFireFloor>(OtherActor))
					{
						FireFloor->On_Fire(); //화염 킴
						//UE_LOG(LogTemp,Warning,TEXT("FireFloor activated!"));
					}
				}
				if(OtherActor->ActorHasTag("FrozeFloor"))
				{
					if(AFrozeFloor* FrozeFloor = Cast<AFrozeFloor>(OtherActor))
					{
						FrozeFloor->Off_Froze(); //빙결 킴 코드
						//	UE_LOG(LogTemp,Warning,TEXT("FrozeFloor activated!"));

					}
				}
				if(OtherActor->ActorHasTag("FireCrystal"))
				{
					UE_LOG(LogTemp,Warning,TEXT("FireCrystal activated!"));
					OtherActor->Destroy();
				}
				break;
			}

			}

			Destroy(); // 충돌 후 제거
		}

	}
}
void APlayerProjectile::ApplyFireDOT(AActor* Target,float DamagePerSecond,float ApplyDuration)
{
	if(!Target || DamagePerSecond <= 0.f || ApplyDuration <= 0.f) return;
	if(Target){
		int32 TickCount = FMath::FloorToInt(ApplyDuration);
		for(int32 i = 1; i <= TickCount; ++i)
		{
			FTimerHandle FireTickHandle;
			// [캡처](매개변수)->Return Type{ 구현 몸체 } 
			FTimerDelegate FireTickDelegate = FTimerDelegate::CreateLambda([=,this]()
			{
				UGameplayStatics::ApplyDamage(Target,DamagePerSecond,GetInstigatorController(),this,nullptr);
				//UE_LOG(LogTemp,Warning,TEXT("ApplyFireDOT: %f damage to %s (tick %d)"),DamagePerSecond,*Target->GetName(),i);
			});
			GetWorld()->GetTimerManager().SetTimer(FireTickHandle,FireTickDelegate,i,false);
		}
	}
}

void APlayerProjectile::OnLifetimeExpired()
{
	Destroy(); // 또는 ReturnToPool();
}
