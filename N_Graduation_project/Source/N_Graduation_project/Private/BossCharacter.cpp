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
	CurrentHP=BossHP;

	LaserSocketNames = {"Red1","Red2","Red3","Red4"};
	static ConstructorHelpers::FClassFinder<AActor> LaserBPClassFinder(TEXT("/Game/VFX/BP_Laser"));
	if(LaserBPClassFinder.Succeeded())
	{
		LaserBPClass = LaserBPClassFinder.Class;
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
	UpdateHP();
	Pattern1();
}

void ABossCharacter::Pattern1(){
	///	if(SkillStart){

	//BossPatternManager->Thunder();
	UE_LOG(LogTemp,Warning,TEXT("Thunde Pattern1"));

	//얼마나 걸릴까
	float Timer=2.0f;
	FTimerHandle LaserDelayHandle;
	GetWorldTimerManager().SetTimer(LaserDelayHandle,this,&ABossCharacter::SpawnAndAttachLasers,Timer,false);
	//}
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


	if(CurrentHP == 0)
	{
		OnBossDead();
	}
}
void ABossCharacter::UpdateHP(){
	BossWidget->UpdateHPBar(CurrentHP);
}

void ABossCharacter::OnBossDead(){
	Destroy();
}
void ABossCharacter::SpawnAndAttachLasers()
{
	auto* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if(!MyGameInstance) return;
	if(!LaserBPClass) return;
	MyGameInstance->Laser=true;
	for(const FName& SocketName : LaserSocketNames)
	{
		FTransform SocketTransform = GetMesh()->GetSocketTransform(SocketName);

		AActor* Laser = GetWorld()->SpawnActor<AActor>(LaserBPClass,SocketTransform);
		if(!Laser) continue;
		Laser->AttachToComponent(GetMesh(),FAttachmentTransformRules::SnapToTargetNotIncludingScale,SocketName);

		// 1초 후 소켓에 부착 및 레이저 초기화
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this,Laser,SocketName]()
		{
			if(!Laser) return;

			// 디태치
		//	Laser->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			auto* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
			if(!MyGameInstance) return;
			//MyGameInstance->Laser=false;
			//Laser->Destroy();
		},4.0f,false);
	}
}