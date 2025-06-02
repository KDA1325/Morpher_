#include "HealCrystal.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"
#include "BossCharacter.h" 
#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
// Called when the game starts or when spawned
AHealCrystal::AHealCrystal(){
	BossHealSound =  LoadObject<USoundWave>(nullptr,TEXT("SoundWave'/Game/Sounds/Battle/BossHeal.BossHeal'"));
	BossBreakSound =  LoadObject<USoundWave>(nullptr,TEXT("SoundWave'/Game/Sounds/Battle/BreakObject.BreakObject'"));
	BossOverTimeSound =  LoadObject<USoundWave>(nullptr,TEXT("SoundWave'/Game/Sounds/Battle/HealOverTime.HealOverTime'"));

}
void AHealCrystal::BeginPlay()
{
	Super::BeginPlay();

}

void AHealCrystal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHealCrystal::StartCrystal(float LimitTime)
{
	GetWorld()->GetTimerManager().SetTimer(
		CrystalExpireTimerHandle,
		this,
		&AHealCrystal::OnCrystalTimeExpired,
		LimitTime,
		false
	);
	NomalHeal();

}

void AHealCrystal::OnCrystalTimeExpired()
{
	ABossCharacter* BossChar = Cast<ABossCharacter>(BossActor);
	auto* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if(!MyGameInstance) return;

	if(BossChar && !bIsDestroyed)
	{
		BossChar->HealHP(200);

		if(BossHealSound)
		{
			UE_LOG(LogTemp,Warning,TEXT("BossHealSound 로드 성공"));

			UGameplayStatics::SpawnSoundAtLocation(this,BossHealSound,GetActorLocation());
		} else
		{
			UE_LOG(LogTemp,Error,TEXT("BossHealSound == nullptr"));
		}
		Destroy();

	} else
	{
		// 파괴된 경우 즉시 삭제
		UGameplayStatics::SpawnSoundAtLocation(this,BossBreakSound,GetActorLocation());
		Destroy();
	}

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	MyGameInstance->BossHeal = false;
}
float AHealCrystal::TakeDamage(float DamageAmount,FDamageEvent const& DamageEvent,AController* EventInstigator,AActor* DamageCauser)
{
	UE_LOG(LogTemp,Warning,TEXT("AHealCrystal TakeDamage"));

	SetHP(currentHP - DamageAmount);
	return DamageAmount;
}

void AHealCrystal::SetHP(int NewHP)
{
	UE_LOG(LogTemp,Warning,TEXT("AHealCrystal damage받음"));
	currentHP = NewHP;
	if(currentHP <= 0)
	{
		UGameplayStatics::SpawnSoundAtLocation(this,BossBreakSound,GetActorLocation());
		bIsDestroyed=true;
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		GetWorld()->GetTimerManager().ClearTimer(HealLoopTimerHandle);
		UE_LOG(LogTemp,Warning,TEXT("플레이어에 의해 수정체가 파괴됨"));
	}
}

void AHealCrystal::NomalHeal(){
	ABossCharacter* BossChar = Cast<ABossCharacter>(BossActor);
	if(BossChar)
	{
		if(bIsDestroyed==false)
		{
			BossChar->HealHP(HealAmountPerSecond);
			UE_LOG(LogTemp,Log,TEXT("NomalHeal: +%d"),HealAmountPerSecond);

			UGameplayStatics::SpawnSoundAtLocation(this,BossOverTimeSound,GetActorLocation());

			GetWorld()->GetTimerManager().SetTimer(
			HealLoopTimerHandle,
			this, 
			&AHealCrystal::NomalHeal,
			HealInterval,
			false  // false = 단발성 타이머로 매 호출 시 재설정
			);
		} else
		{

		}
	}
}
