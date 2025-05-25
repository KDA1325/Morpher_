// Fill out your copyright notice in the Description page of Project Settings.


#include "Totem.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Totem_Projectile.h"
#include "Components/ArrowComponent.h"// Sets default values
#include "EntityPreset.h"// Sets default values

ATotem::ATotem()
{
	PrimaryActorTick.bCanEverTick = true;

	FaceComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("FaceComponent"));
	FaceComponent->SetupAttachment(RootComponent);

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	TotemHP=0;
	AttackDelay=0;
	ProjectileSpeed=0;
	ApplyDamageAmount=0;
}

// Called when the game starts or when spawned
void ATotem::BeginPlay()
{
	Super::BeginPlay();
	// 일정 시간마다 FireProjectile 호출
	GetWorldTimerManager().SetTimer(AttackTimerHandle,this,&ATotem::TotemProjectile,AttackDelay,true);
	TotemProjectile();
}


void ATotem::TotemProjectile()
{
	if(!ProjectileClass) 	{
		UE_LOG(LogTemp,Error,TEXT("ATotem_Projectile ProjectileClass not set!"));
		return;
	}

	FVector SpawnLocation = FaceComponent->GetComponentLocation();
	FVector Direction = FaceComponent->GetForwardVector();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	// 투사체 액터 스폰 
	FRotator SpawnRotation = FaceComponent->GetComponentRotation();
	ATotem_Projectile* SpawnedProjectile = GetWorld()->SpawnActor<ATotem_Projectile>(ProjectileClass,SpawnLocation,SpawnRotation,SpawnParams);
	UE_LOG(LogTemp,Warning,TEXT("ATotem_Projectile TotemProjectile()실행됨"));

	if(SpawnedProjectile)
	{
		// Skill 데이터 테이블에서 "Skill_ThrowRock" 데이터 가져오기
		FSkillData SkillData;
		TArray<FSkillEffectData> EffectDataArray;

		if(UABGameSingleton::Get().GetSkillDataBySkillID("Skill_ThrowRock",SkillData) &&
			UABGameSingleton::Get().GetSkillEffectDataBySkillID("Skill_ThrowRock",EffectDataArray))
		{
			// 초기화
			SpawnedProjectile->InitProjectileBySkillData(ProjectileSpeed,ApplyDamageAmount);

			//SpawnedProjectile->IgnoreActorWhenMoving(this,true);

			UE_LOG(LogTemp,Error,TEXT("Spawned ThrowRock Projectile"));

			SpawnedProjectile->FireInDirection(Direction);
		} else
		{
			UE_LOG(LogTemp,Error,TEXT("Failed to load Skill_ThrowRock data!"));
		}
	}


}

float ATotem::TakeDamage(float DamageAmount,FDamageEvent const& DamageEvent,AController* EventInstigator,AActor* DamageCauser)
{
	TotemHP -= DamageAmount;

	if(TotemHP <= 0.0f)
	{
		Destroy();
	}
	//UE_LOG(LogTemp,Log,TEXT("Damage Totem %s"),DamageAmount)

		return DamageAmount;
}
