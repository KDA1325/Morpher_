#include "BossCharacter.h"
#include "ManaCoreCrystal.h"
#include "Kismet/GameplayStatics.h"
#include "BossWidget.h"
#include "Blueprint/UserWidget.h"
#include "Boss_Laser.h" 
#include "MyGameInstance.h"

ABossCharacter::ABossCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FClassFinder<UUserWidget> Boss(TEXT("WidgetBlueprint'/Game/Entity/Boss/BP_BossHPWidget.BP_BossHPWidget_C'"));
	if(Boss.Succeeded())
	{
		BossClass = Boss.Class;
	}
	//CurrentHP=BossHP;
	CurrentHP=200;
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

void ABossCharacter::Pattern1(){
	UE_LOG(LogTemp,Warning,TEXT("Pattern1 실행"));

	//if(SkillStart==true){


		//BossPatternManager->Thunder();
	UE_LOG(LogTemp,Warning,TEXT("Thunde Pattern1"));

	//	BossPatternManager->SpawnAndAttachLasers();

	//if(BossPatternManager)
	//{
	//	float Timer = 1.0f;
	//	FTimerHandle LaserDelayHandle;
	//	GetWorld()->GetTimerManager().SetTimer(LaserDelayHandle,
	//		FTimerDelegate::CreateUObject(BossPatternManager,&ABossPatternManager::SpawnAndAttachLasers),
	//		Timer,false);
	//}

	UE_LOG(LogTemp,Warning,TEXT("Pattern1 실행"));

	//	BossPatternManager->StartSpinningBarrageSequence(5);
	BossPatternManager->HealCrystal();

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

}
void ABossCharacter::UpdateHP(){
	BossWidget->UpdateHPBar(CurrentHP);
}

void ABossCharacter::OnBossDead(){
	Destroy();
}
