// Fill out your copyright notice in the Description page of Project Settings.


#include "EntityProjectile.h"
#include "ABGameSingleton.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NormalAttackDamageType.h"
#include "N_Graduation_project/N_Graduation_projectCharacter.h"
// Sets default values
AEntityProjectile::AEntityProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 발사체 이동 컴포넌트 설정
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 0.f;
	ProjectileMovement->MaxSpeed = 0.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	//ProjectileMovement->Activate();

	// 변수 초기화 
	Damage = 0.f;
	AOERadius = 0.f;
	bApplyFireDot = false;
	FireDamage = 0.f;
	FireDuration = 0.f;
	SoundSkillID = nullptr;
	//static ConstructorHelpers::FObjectFinder<USoundBase>FireEffectHitSoundObj(TEXT("/Script/Engine.SoundWave'/Game/Sounds/Battle/FireBall.FireBall'"));
	//if(FireEffectHitSoundObj.Succeeded())
	//{
	//	FireEffectHitSound = FireEffectHitSoundObj.Object;
	//}

}

// Called when the game starts or when spawned
void AEntityProjectile::BeginPlay()
{
	Super::BeginPlay();


	/*if(!CollisionComp)
	{
		CollisionComp = FindComponentByClass<USphereComponent>();
	}*/

	CollisionComp = Cast<UPrimitiveComponent>(FindComponentByClass<USphereComponent>());
	if(!CollisionComp)
	{
		CollisionComp = Cast<UPrimitiveComponent>(FindComponentByClass<UBoxComponent>());
	}

	if(CollisionComp && ProjectileMovement)
	{
		UE_LOG(LogTemp,Warning,TEXT("CollisionComp found: %s"),*CollisionComp->GetName());

		RootComponent = CollisionComp;

		ProjectileMovement->SetUpdatedComponent(CollisionComp);
		//ProjectileMovement->Activate(true);

		if(bAutoFireOnSpawn)
		{
			ProjectileMovement->Activate(true);
		} else
		{
			//FRotator FixRotation = GetActorRotation();
			//FixRotation = FRotator(0.f,90.f,0.f); 
			//SetActorRotation(FixRotation);
		}

		CollisionComp->OnComponentBeginOverlap.AddDynamic(this,&AEntityProjectile::OnOverlap);
	} else
	{
		UE_LOG(LogTemp,Error,TEXT("Hitbox is null in BeginPlay! Check BP binding."));
	}
}

// Called every frame
void AEntityProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEntityProjectile::InitProjectileBySkillData(const FSkillData & InSkillData,const TArray<FSkillEffectData>& InEffectData)
{
	SkillData = InSkillData;
	EffectDataArray = InEffectData;
	SkillRange = InSkillData.SkillRange;

	//// 발사 속도 설정
	//if(ProjectileMovement)
	//{
	//	ProjectileMovement->InitialSpeed = SkillData.ProjectileSpeed;
	//	ProjectileMovement->MaxSpeed = SkillData.ProjectileSpeed;

	//	//// 충돌 무시 설정
	//	//if(GetOwner())
	//	//{
	//	//	ProjectileMovement->IgnoreActorWhenMoving(GetOwner(),true);
	//	//}
	//}

	if(CollisionComp && GetOwner())
	{
		CollisionComp->IgnoreActorWhenMoving(GetOwner(),true);
	}

	//// SkillRange까지 도달 후 투사체 삭제를 위한 타이머 
	//if(SkillData.ProjectileSpeed > 0.f)
	//{
	//	float LifeTime = SkillData.SkillRange / SkillData.ProjectileSpeed;
	//	GetWorld()->GetTimerManager().SetTimer(
	//		DestroyTimerHandle,
	//		this,
	//		&AEntityProjectile::OnLifetimeExpired,
	//		LifeTime,
	//		false
	//	);
	//}
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
			if(Effect.SkillNameID=="Skill_ThrowRock"||Effect.SkillNameID=="Skill_Arrow")
			{
				UE_LOG(LogTemp,Error,TEXT("무적 모드 프로젝트타일"));

				UGameplayStatics::ApplyDamage(OtherActor,Effect.EffectValue01,GetInstigatorController(),this,UNormalAttackDamageType::StaticClass());
			}
			else if(Effect.SkillNameID=="Skill_FireBall")
			{
				SoundSkillID = "Skill_FireBall";
				UGameplayStatics::ApplyDamage(OtherActor,Effect.EffectValue01,GetInstigatorController(),this,nullptr);
				SoundSkillID = nullptr;
			}
			else
			{
				UE_LOG(LogTemp,Error,TEXT("무적 모드 아님"));

				UGameplayStatics::ApplyDamage(OtherActor,Effect.EffectValue01,GetInstigatorController(),this,nullptr);
			}
			break;
			case EnumEffectType::AOEDamage:
			// 광역 대미지 처리, 몬스터용 로직엔 추가할 필요 없을 듯(플레이어 쪽에 추가하기)
			UGameplayStatics::ApplyDamage(OtherActor,Effect.EffectValue01,GetInstigatorController(),this,nullptr);
			break;
			//FVector Origin = GetActorLocation();
			//float Radius = Effect.EffectValue02;
			//TArray<AActor*> OverlappingActors;
			//UGameplayStatics::GetAllActorsOfClass(GetWorld(),AN_Graduation_projectCharacter::StaticClass(),OverlappingActors);

			//for(AActor* Actor : OverlappingActors)
			//{
			//	if(Actor->ActorHasTag("Player"))
			//	{
			//		float Distance = FVector::Dist(Actor->GetActorLocation(),Origin);
			//		if(Distance <= Radius)
			//		{
			//			UGameplayStatics::ApplyDamage(Actor,Effect.EffectValue01,GetInstigatorController(),this,nullptr);
			//			UE_LOG(LogTemp,Warning,TEXT("AOE Damage to %s"),*Actor->GetName());
			//		}
			//	}
			//}
			case EnumEffectType::Fire:
			// 불 디버프 적용
			AN_Graduation_projectCharacter* PlayerCharacter = Cast<AN_Graduation_projectCharacter>(OtherActor);
			if(!PlayerCharacter) return;
			float ApplyDuration = Effect.EffectValue01;
			float DPS = Effect.EffectValue02;
			PlayerCharacter->ApplyFire(ApplyDuration);
			ApplyFireDOT(OtherActor,DPS,ApplyDuration);
			break;
			}
		}

		if(ProjectileHitEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),ProjectileHitEffect,SweepResult.ImpactPoint,SweepResult.ImpactNormal.Rotation());
			//ProjectileHitEffect->SetWorldScale3D(FVector(1.0f));
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

		ProjectileMovement->InitialSpeed = SkillData.ProjectileSpeed;
		ProjectileMovement->MaxSpeed = SkillData.ProjectileSpeed;

		ProjectileMovement->SetUpdatedComponent(CollisionComp);
		ProjectileMovement->SetVelocityInLocalSpace(ShootDirection * ProjectileMovement->InitialSpeed);
		ProjectileMovement->Activate(true);

		UE_LOG(LogTemp,Warning,TEXT("FireInDirection called! Velocity = %s"),*ProjectileMovement->Velocity.ToString());

		// SkillRange까지 도달 후 투사체 삭제를 위한 타이머 
		if(SkillData.ProjectileSpeed > 0.f)
		{
			float LifeTime = SkillData.SkillRange / SkillData.ProjectileSpeed;
			GetWorld()->GetTimerManager().SetTimer(
				DestroyTimerHandle,
				this,
				&AEntityProjectile::OnLifetimeExpired,
				LifeTime,
				false
			);
		}
	}
}

void AEntityProjectile::ApplyFireDOT(AActor* Target,float DamagePerSecond,float ApplyDuration)
{
	if(!Target || DamagePerSecond <= 0.f || ApplyDuration <= 0.f) return;

	int32 TickCount = FMath::FloorToInt(ApplyDuration);
	for(int32 i = 1; i <= TickCount; ++i)
	{
		int32 TickNumber = i;
		FTimerHandle FireTickHandle;

		FTimerDelegate FireTickDelegate = FTimerDelegate::CreateLambda([=,this]() {
			UGameplayStatics::ApplyDamage(Target,DamagePerSecond,GetInstigatorController(),this,nullptr);
			UE_LOG(LogTemp,Warning,TEXT("ApplyFireDOT: %f damage to %s (tick %d)"),DamagePerSecond,*Target->GetName(),TickNumber);
		});

		//Entity->PlayFireSound();

		GetWorld()->GetTimerManager().SetTimer(FireTickHandle,FireTickDelegate,i,false);
	}
}

//void AEntityProjectile::ApplyFireDOT(AActor* Target,float DamagePerSecond,float ApplyDuration)
//{
//	if(!Target || DamagePerSecond <= 0.f || ApplyDuration <= 0.f) return;
//
//	int32 TickCount = FMath::FloorToInt(ApplyDuration);
//	for(int32 i = 1; i <= TickCount; ++i)
//	{
//		FTimerHandle FireTickHandle;
//
//		// [캡처](매개변수)->Return Type{ 구현 몸체 } 
//		/*FTimerDelegate FireTickDelegate = FTimerDelegate::CreateLambda([=, this]()
//		{
//			UGameplayStatics::ApplyDamage(Target, DamagePerSecond, GetInstigatorController(), this, nullptr);
//			UE_LOG(LogTemp,Warning,TEXT("ApplyFireDOT: %f damage to %s (tick %d)"), DamagePerSecond, *Target->GetName(), i);
//		});
//		GetWorld()->GetTimerManager().SetTimer(FireTickHandle,FireTickDelegate,i,false);*/
//
//		FTimerDelegate FireTickDelegate = FTimerDelegate::CreateLambda([=,this]()
//		{
//			if(IsValid(Target))
//			{
//				UGameplayStatics::ApplyDamage(Target, DamagePerSecond, GetInstigatorController(), this, nullptr);
//				UE_LOG(LogTemp, Warning, TEXT("Fire DOT Tick %f to %s (tick %d)"), DamagePerSecond, *Target->GetName(), i);
//			}
//		});
//
//		// 효과 타이머 중첩을 위해 
//		// i초 후 해당 tick 실행 (총 TickCount번)
//		GetWorld()->GetTimerManager().SetTimer(
//			FireTickHandle,
//			FireTickDelegate,
//			i,  // 지연 시간: i초 후 실행
//			false  // 한 번만 실행
//		);
//	}
//}

void AEntityProjectile::OnLifetimeExpired()
{
	Destroy(); // 또는 ReturnToPool();
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
