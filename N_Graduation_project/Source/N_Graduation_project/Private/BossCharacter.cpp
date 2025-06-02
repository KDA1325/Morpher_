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
	static ConstructorHelpers::FClassFinder<AActor> SpinBP2ClassFinder(TEXT("/Game/Entity/Boss/Boss_Projectile1"));
	if(SpinBP2ClassFinder.Succeeded())
	{
		SpinningBP2Class = SpinBP2ClassFinder.Class;
	}	static ConstructorHelpers::FClassFinder<AActor> SpinBP2_2ClassFinder(TEXT("/Game/Entity/Boss/Boss_Projectile1"));
	if(SpinBP2_2ClassFinder.Succeeded())
	{
		SpinningBP2_2Class = SpinBP2_2ClassFinder.Class;
	}
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
	auto* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if(!MyGameInstance) return;
	MyGameInstance->part2=false;
	Pattern1();
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
	auto* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if(!MyGameInstance) return;
	//	PastCurrentHP = CurrentHP;
	CurrentHP = NewHP;
	UpdateHP();
	UE_LOG(LogTemp,Log,TEXT("CurrentHP HealHP(200) 결과: %d"),CurrentHP);

	if(CurrentHP <= 0)
	{
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
		if(BossPatternManager)
		{
			GetWorld()->GetTimerManager().ClearAllTimersForObject(BossPatternManager);
		}

		// 위젯 제거
		if(BossWidget)
		{
			BossWidget->RemoveFromParent();
			BossWidget = nullptr;
		}
		MyGameInstance->BossClear=true;
		// 보스 액터 파괴
		Destroy();
	}

}
void ABossCharacter::HealHP(int DamageAmount){
	CurrentHP = FMath::Clamp(CurrentHP + DamageAmount,0,BossHP); // 안전하게 조정
	SetHP(CurrentHP); // 이걸로 UI까지 갱신되도록	SetHP(CurrentHP);
	if(HealEffect)
	{
		//UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		//	GetWorld(),
		//	HealEffect,
		//	GetActorLocation() + FVector(0,0,-500),
		//	FRotator::ZeroRotator,//회전
		//	FVector(100.0f),  //스케일
		//	true,           // Auto Destroy
		//	true            // Auto Activate
		//);
	}
}
void ABossCharacter::UpdateHP(){
	if(BossWidget){
		BossWidget->UpdateHPBar(CurrentHP);
	}
}

//페이즈
void ABossCharacter::Pattern1()
{
	if(!BossPatternManager) return;
	//번개
	BossPatternManager->Thunder();
	//BossPatternManager->Meteor();

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	//레이저
	TimerManager.SetTimer(LaserDelayHandle,FTimerDelegate::CreateUObject(BossPatternManager,&ABossPatternManager::SpawnAndAttachLasers),7.f,false);
	//투사체
	GetWorld()->GetTimerManager().SetTimer(SpinDelayHandle,FTimerDelegate::CreateLambda([this]()
	{
		BossPatternManager->StartSpinningBarrageSequence(6);
	}),16.f,false);
	//회복
	TimerManager.SetTimer(HealDelayHandle,FTimerDelegate::CreateUObject(BossPatternManager,&ABossPatternManager::HealCrystal),30.f,false);

	GetWorld()->GetTimerManager().SetTimer(
		PatternLoopHandle,
		this,
		&ABossCharacter::ExecuteBossPattern,
		47.f,false
	);


}

void ABossCharacter::ExecuteBossPattern()
{
	auto* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if(!MyGameInstance) return;

	if(!BossPatternManager) return;
	if(CurrentHP <= 500 && !bPhase2Started)
	{
		bPhase2Started = true;
		Pattern2();
		return; // Pattern2 실행 이후 다른 패턴 실행 안 함
	}
	int32 PatternIndex = FMath::RandRange(0,4);
	float Delay = 0.f;

	switch(PatternIndex)
	{
	case 0:
	BossPatternManager->Thunder();
	MyGameInstance->Laser=false;
	Delay = 7.f;
	break;
	case 1:
	BossPatternManager->SpawnAndAttachLasers();
	Delay = 8.f;
	break;
	case 2:
	MyGameInstance->Laser=false;
	BossPatternManager->StartSpinningBarrageSequence(5);

	Delay = 10.f;
	break;
	case 3:
	MyGameInstance->Laser=false;
	BossPatternManager->HealCrystal();
	Delay = 16.f;
	break;
	default:
	UE_LOG(LogTemp,Warning,TEXT("Invalid PatternIndex: %d"),PatternIndex);
	Delay = 3.f;
	break;
	}

	// 다음 패턴 예약
	GetWorld()->GetTimerManager().SetTimer(
		PatternLoopHandle,
		this,
		&ABossCharacter::ExecuteBossPattern,
		Delay,false
	);
}

void ABossCharacter::Pattern2(){
	//메테오
	//캐릭터 붉게 만들고
	//auto* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	//if(!MyGameInstance) return;
	//MyGameInstance->part2=true;
	BossPatternManager->Meteor();

	UE_LOG(LogTemp,Log,TEXT("Pattern2 실행"));
	GetWorld()->GetTimerManager().SetTimer(
	PatternLoopHandle,
	this,
	&ABossCharacter::ExecuteBossPattern2,
	6.f,false
	);
}
void ABossCharacter::ExecuteBossPattern2()
{
	auto* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if(!MyGameInstance) return;

	if(!BossPatternManager) return;

	int32 PatternIndex = FMath::RandRange(0,4);
	float Delay = 0.f;

	switch(PatternIndex)
	{
	case 0:
	MyGameInstance->Laser=false;
	BossPatternManager->ThunderCount=9;
	BossPatternManager->Delay=0.8f;
	BossPatternManager->Thunder();
	Delay = 10.f;
	break;
	case 1:
	BossPatternManager->SpawnAndAttachLasers();
	Delay = 8.f;
	break;
	case 2:
	MyGameInstance->Laser=false;

	BossPatternManager->StartSpinningBarrageSequence2(5);
	Delay = 10.f;
	break;
	case 3:
	MyGameInstance->Laser=false;

	BossPatternManager->Meteor();
	Delay = 7.f;
	break;
	case 4:
	MyGameInstance->Laser=false;

	BossPatternManager->HealCrystal();
	Delay = 16.f;
	break;
	default:
	UE_LOG(LogTemp,Warning,TEXT("Invalid PatternIndex: %d"),PatternIndex);
	Delay = 3.f;
	break;
	}

	// 다음 패턴 예약
	GetWorld()->GetTimerManager().SetTimer(
		PatternLoopHandle2,
		this,
		&ABossCharacter::ExecuteBossPattern2,
		Delay,false
	);
}
