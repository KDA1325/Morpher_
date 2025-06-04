#include "Totem.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Totem_Projectile.h"
#include "Components/ArrowComponent.h"
#include "EntityPreset.h"

ATotem::ATotem()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = Root;

	FaceComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("FaceComponent"));
	FaceComponent->SetupAttachment(RootComponent);
	FaceComponent->SetRelativeRotation(FRotator(0.f,0.f,0.f)); // X+ 방향 정렬

	TotemHP = 100.f;
	AttackDelay = 2.f;
	ProjectileSpeed = 1500.f;
	ApplyDamageAmount = 20.f;

	static ConstructorHelpers::FObjectFinder<USoundBase>BreakObjectObj(TEXT("/Script/Engine.SoundWave'/Game/Sounds/Battle/BreakObject.BreakObject'"));
	if(BreakObjectObj.Succeeded())
	{
		BreakObjectSound = BreakObjectObj.Object;
	}
}

void ATotem::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(AttackTimerHandle,this,&ATotem::TotemProjectile,AttackDelay,true);
	TotemProjectile();
}

void ATotem::TotemProjectile()
{
	if(!ProjectileClass)
	{
		UE_LOG(LogTemp,Error,TEXT("ATotem_Projectile ProjectileClass not set!"));
		return;
	}

	FVector SpawnLocation = FaceComponent->GetComponentLocation();
	FVector Direction = FaceComponent->GetForwardVector();
	FRotator SpawnRotation = Direction.Rotation();

	UE_LOG(LogTemp,Warning,TEXT("Totem Fire Direction: %s"),*Direction.ToString());

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	ATotem_Projectile* SpawnedProjectile = GetWorld()->SpawnActor<ATotem_Projectile>(
		ProjectileClass,SpawnLocation,SpawnRotation,SpawnParams);

	if(SpawnedProjectile)
	{
		FSkillData SkillData;
		TArray<FSkillEffectData> EffectDataArray;

		if(UABGameSingleton::Get().GetSkillDataBySkillID("Skill_ThrowRock",SkillData) &&
			UABGameSingleton::Get().GetSkillEffectDataBySkillID("Skill_ThrowRock",EffectDataArray))
		{
			SpawnedProjectile->InitProjectileBySkillData(ProjectileSpeed,ApplyDamageAmount);
			SpawnedProjectile->FireInDirection(Direction);

			UE_LOG(LogTemp,Log,TEXT("Spawned ThrowRock Projectile"));
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
		UGameplayStatics::SpawnSoundAtLocation(this,BreakObjectSound,GetOwner()->GetActorLocation());
		Destroy();
	}

	return DamageAmount;
}
