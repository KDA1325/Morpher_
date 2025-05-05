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

	//CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	//CollisionComp->InitSphereRadius(15.0f);
	//CollisionComp->SetCollisionProfileName("BlockAllDynamic");
	//RootComponent = CollisionComp;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	//ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 600.f;
	ProjectileMovement->MaxSpeed = 600.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;

	//CollisionComp->OnComponentHit.AddDynamic(this,&AEntityProjectile::OnHit);

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
	
	if(CollisionComp)
	{
		ProjectileMovement->UpdatedComponent = CollisionComp;
		CollisionComp->OnComponentHit.AddDynamic(this,&AEntityProjectile::OnHit);
	} else
	{
		UE_LOG(LogTemp,Error,TEXT("CollisionComp is null in BeginPlay! Check BP binding."));
	}
}

// Called every frame
void AEntityProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
//
//void AEntityProjectile::InitProjectile(const FSkillData& SkillData,const TArray<FSkillEffectData>& EffectData)
//{
//	// 기본으로 들어갈 데이터 -> 데이터를 불러왔는데도 값이 0이면 해당 효과 부여 X
//	Damage = 0.0f;
//	AOERadius = 0.0f;
//	bApplyFireDot = false;
//	FireDamage = 0.0f;
//	FireDuration = 0.0f;
//
//	// SkillEffectData를 분석해서 적용
//	for(const FSkillEffectData& Effect : EffectData)
//	{
//		switch(Effect.EffectType)
//		{
//		case EnumEffectType::Damage:
//		Damage = Effect.EffectValue01;
//		break;
//
//		case EnumEffectType::AOEDamage:
//		Damage = Effect.EffectValue01;
//		AOERadius = Effect.EffectValue02;
//		break;
//
//		case EnumEffectType::Fire:
//		bApplyFireDot = true;
//		FireDamage = Effect.EffectValue01;
//		FireDuration = Effect.EffectValue02;
//		break;
//
//		default:
//		break;
//		}
//	}
//
//	
//	if(ProjectileMovement)
//	{
//		ProjectileMovement->InitialSpeed = SkillData.ProjectileSpeed;
//		ProjectileMovement->MaxSpeed = SkillData.ProjectileSpeed;
//	}
//}

void AEntityProjectile::InitProjectileBySkillData(const FSkillData & InSkillData, const TArray<FSkillEffectData>& InEffectData)
{
	SkillData = InSkillData;
	EffectDataArray = InEffectData;

	// 발사 속도 설정
	if(ProjectileMovement)
	{
		ProjectileMovement->InitialSpeed = SkillData.ProjectileSpeed;
		ProjectileMovement->MaxSpeed = SkillData.ProjectileSpeed;
	}
}

void AEntityProjectile::OnHit(UPrimitiveComponent* HitComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,FVector NormalImpulse,const FHitResult& Hit)
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
}

void AEntityProjectile::FireInDirection(const FVector& ShootDirection)
{
	if(ProjectileMovement)
	{
		ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
	}
}

//void AEntityProjectile::OnProjectileHit(UPrimitiveComponent* HitComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,FVector NormalImpulse,const FHitResult& Hit)
//{
//	// 데미지 적용
//	if(Damage > 0 && OtherActor && OtherActor != this)
//	{
//		if(AOERadius > 0)
//		{
//			// AOE 데미지 처리
//			UGameplayStatics::ApplyRadialDamage(this,Damage,GetActorLocation(),AOERadius,nullptr,TArray<AActor*>(),this,GetInstigatorController(),true);
//		} else
//		{
//			UGameplayStatics::ApplyDamage(OtherActor,Damage,GetInstigatorController(),this,nullptr);
//		}
//	}
//
//	// Fire DoT 적용 예정 (추가 구현)
//	if(bApplyFireDot && OtherActor)
//	{
//		// 예: 커스텀 인터페이스, 상태이상 컴포넌트 등을 통해 화상 상태 부여
//		// 이 부분은 추후 캐릭터에 상태이상 시스템이 있으면 구현
//	}
//
//	Destroy();
//}