#include "BossCharacter.h"
#include "ManaCoreCrystal.h"
#include "Kismet/GameplayStatics.h"
#include "BossWidget.h"
#include "Blueprint/UserWidget.h"
#include "Boss_Laser.h" 
#include "MyGameInstance.h"
#include "NiagaraFunctionLibrary.h"  

ABossCharacter::ABossCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FClassFinder<UUserWidget> Boss(TEXT("WidgetBlueprint'/Game/Entity/Boss/BP_BossHPWidget.BP_BossHPWidget_C'"));
	if(Boss.Succeeded())
	{
		BossClass = Boss.Class;
	}
	CurrentHP=BossHP;
	//CurrentHP=200;
	//BossPatternManager =CreateDefaultSubobject<ABossPatternManager>(TEXT("BossPatternManager"));

}
UBossWidget* ABossCharacter::GetHUD() const
{
	if(BossHPWidgetComponent)
	{
		return Cast<UBossWidget>(BossHPWidgetComponent->GetUserWidgetObject());
	}
	return nullptr;
}

void ABossCharacter::BeginPlay()
{
	Super::BeginPlay();
	if(!BossWidget && BossClass)
	{
		BossWidget = Cast<UBossWidget>(CreateWidget(GetWorld()->GetFirstPlayerController(),BossClass));
		if(BossWidget)
		{
			BossWidget->AddToViewport();
		}
	}
	BossPatternManager = GetWorld()->SpawnActor<ABossPatternManager>(ABossPatternManager::StaticClass(),FVector::ZeroVector,FRotator::ZeroRotator);
	UpdateHP();
	Pattern1();
}
void ABossCharacter::Pattern1()
{
	if(!BossPatternManager) return;
	//번개
	BossPatternManager->Thunder();
	//BossPatternManager->SpawnAndAttachLasers();

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	//레이저
	TimerManager.SetTimer(LaserDelayHandle,FTimerDelegate::CreateUObject(BossPatternManager,&ABossPatternManager::SpawnAndAttachLasers),7.f,false);
	//투사체
	GetWorld()->GetTimerManager().SetTimer(SpinDelayHandle,FTimerDelegate::CreateLambda([this]()
	{
		BossPatternManager->StartSpinningBarrageSequence(5);
	}),14.f,false);
	//메테오
	TimerManager.SetTimer(MDelayHandle,FTimerDelegate::CreateUObject(BossPatternManager,&ABossPatternManager::Meteor),20.f,false);
	//회복
	TimerManager.SetTimer(HealDelayHandle,FTimerDelegate::CreateUObject(BossPatternManager,&ABossPatternManager::HealCrystal),27.f,false);
}
float ABossCharacter::TakeDamage(float DamageAmount,FDamageEvent const& DamageEvent,AController* EventInstigator,AActor* DamageCauser)
{
	// 데미지 로그 출력	
	float FinalDamage = Super::TakeDamage(DamageAmount,DamageEvent,EventInstigator,DamageCauser);
	UE_LOG(LogTemp,Error,TEXT("Boss TakeDamage  %f"),DamageAmount);
	SetHP(CurrentHP - DamageAmount);
	return FinalDamage;
}

void ABossCharacter::SetHP(int NewHP)
{
	//	PastCurrentHP = CurrentHP;
	CurrentHP = NewHP;
	UpdateHP();
	UE_LOG(LogTemp,Log,TEXT("CurrentHP HealHP(200) 결과: %d"),CurrentHP);

	if(CurrentHP == 0)
	{
		OnBossDead();
	}
}
void ABossCharacter::HealHP(int DamageAmount){
	CurrentHP = FMath::Clamp(CurrentHP + DamageAmount,0,BossHP); // 안전하게 조정
	SetHP(CurrentHP); // 이걸로 UI까지 갱신되도록	SetHP(CurrentHP);
	if(HealEffect)
	{

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			HealEffect,
			GetActorLocation() + FVector(0,0,-500),
			FRotator::ZeroRotator,//회전
			FVector(100.0f),  //스케일
			true,           // Auto Destroy
			true            // Auto Activate
		);
	}
}
void ABossCharacter::UpdateHP(){
	BossWidget->UpdateHPBar(CurrentHP);
}

void ABossCharacter::OnBossDead(){
	Destroy();
}
