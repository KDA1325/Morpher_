#include "BossPatternManager.h"
#include "TimerManager.h"
#include "ManaCoreCrystal.h"
#include "Kismet/GameplayStatics.h"
#include "N_Graduation_project/N_Graduation_projectCharacter.h"
#include "MyGameInstance.h"
#include "BossCharacter.h"

ABossPatternManager::ABossPatternManager()
{
	PrimaryActorTick.bCanEverTick = true;
	SphereComponent = nullptr;
	ThunderCount=5;

	LaserSocketNames = {"Red1","Red2","Red3","Red4"};
	static ConstructorHelpers::FClassFinder<AActor> LaserBPClassFinder(TEXT("/Game/VFX/BP_Laser"));
	if(LaserBPClassFinder.Succeeded())
	{
		LaserBPClass = LaserBPClassFinder.Class;
	}

	Spinning1SocketNames = {"core1","core3","core5","core7"};
	Spinning2SocketNames= {"core2","core4","core6","core8"};
		static ConstructorHelpers::FClassFinder<AActor> SpinBPClassFinder(TEXT("/Game/Entity/Boss/Boss_Projectile"));
	if(LaserBPClassFinder.Succeeded())
	{
		SpinningBPClass = SpinBPClassFinder.Class;
	}
}

void ABossPatternManager::BeginPlay()
{
	Super::BeginPlay();
	if(!BossActor)
	{
		BossActor = UGameplayStatics::GetActorOfClass(GetWorld(),ABossCharacter::StaticClass());
		if(BossActor)
		{
			UE_LOG(LogTemp,Warning,TEXT("BossActor 자동 할당됨: %s"),*BossActor->GetName());
		} else
		{
			UE_LOG(LogTemp,Error,TEXT("BossActor 자동 할당 실패! 레벨에 ABossCharacter가 배치되었는지 확인하세요."));
		}
	}
}

void ABossPatternManager::SpawnThunder()
{
	auto* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if(!MyGameInstance) return;
	AN_Graduation_projectCharacter* Player = Cast<AN_Graduation_projectCharacter>(UGameplayStatics::GetPlayerCharacter(this,0));
	if(!Player) return;

	FVector PlayerLocation = Player->GetActorLocation();

	FRotator SpawnRotation = FRotator::ZeroRotator;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();


	GetWorld()->SpawnActor<AActor>(ThunderBPClass,PlayerLocation,SpawnRotation,SpawnParams);
	UE_LOG(LogTemp,Warning,TEXT("Thunder %d번째 소환"),ThunderSpawnCount + 1);
	//if(BossActor)
	//{
	//	FVector BossHeadLocation = BossActor->GetActorLocation() + FVector(0,0,BossHeadHeightOffset); // 머리 위 위치 조절
	//	GetWorld()->SpawnActor<AActor>(ThunderBPClass,BossHeadLocation,SpawnRotation,SpawnParams);
	//	UE_LOG(LogTemp,Warning,TEXT("보스 머리 위 번개 생성"));
	//}
	ThunderSpawnCount++;

	if(ThunderSpawnCount >= ThunderCount)
	{
		UE_LOG(LogTemp,Warning,TEXT("ThunderSpawnCount"));

		GetWorldTimerManager().ClearTimer(ThunderTimerHandle);
		ThunderSpawnCount = 0;  // 초기화 
		MyGameInstance->Thunder = false;

	}

}
 
void ABossPatternManager::Thunder(){
	UE_LOG(LogTemp,Warning,TEXT(" 성공"));

	auto* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if(!MyGameInstance) return;
	UClass* LoadedClass = LoadObject<UClass>(nullptr,TEXT("/Game/Entity/Boss/Boss_Thunder.Boss_Thunder_C"));
	if(LoadedClass)
	{ 
		ThunderBPClass = LoadedClass;
		UE_LOG(LogTemp,Warning,TEXT("ThunderBPClass 동적 로드 성공"));
		
	} else
	{
		UE_LOG(LogTemp,Warning,TEXT("ThunderBPClass 로드 실패"));
	}

	GetWorldTimerManager().SetTimer(ThunderTimerHandle,this,&ABossPatternManager::SpawnThunder,1.0f,true);
	MyGameInstance->Thunder = true;

}

void ABossPatternManager::ApplyThunderDamage()
{
	SphereComponent = FindComponentByClass<USphereComponent>();
	if(SphereComponent)
	{
		SphereComponent->SetGenerateOverlapEvents(true);
		UE_LOG(LogTemp,Log,TEXT("Thunder: SphereComponent 세팅 완료"));
	} else
	{
		UE_LOG(LogTemp,Error,TEXT("Thunder: SphereComponent가 NULL입니다! BP에서 설정되었는지 확인하세요"));
	}

	TArray<AActor*> OverlappingActors;
	SphereComponent->GetOverlappingActors(OverlappingActors);

	for(AActor* Actor : OverlappingActors)
	{
		if(!Actor || Actor == this)
			continue;

		UE_LOG(LogTemp,Warning,TEXT("폭발 범위 내 감지된 액터: %s"),*Actor->GetName());

		if(Actor->ActorHasTag(FName("Player")))
		{
			// 데미지 처리
			AController* InstigatorController = GetInstigatorController();
			UGameplayStatics::ApplyDamage(Actor,ThunderDamage,InstigatorController,this,nullptr);
		}
	}

}
void ABossPatternManager::SpawnAndAttachLasers()
{
	auto* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if(!MyGameInstance) return;
	MyGameInstance->Laser=true;
	UE_LOG(LogTemp,Warning,TEXT("SpawnAndAttachLasers"));

	if(!LaserBPClass) {
		UE_LOG(LogTemp,Warning,TEXT("SpawnAndAttachLasers LaserBPClass 없음"));

		return;
	}
	ABossCharacter* BossChar = Cast<ABossCharacter>(BossActor); 
	if(!BossChar) {
		UE_LOG(LogTemp,Warning,TEXT("SpawnAndAttachLasers BossChar 없.음.%s"),*GetNameSafe(BossActor));
		return; 
	}
	for(const FName& SocketName : LaserSocketNames)
	{ 
		FTransform SocketTransform =BossChar-> GetMesh()->GetSocketTransform(SocketName);

		AActor* Laser = GetWorld()->SpawnActor<AActor>(LaserBPClass,SocketTransform);
		if(!Laser) continue;
		Laser->AttachToComponent(BossChar->GetMesh(),FAttachmentTransformRules::SnapToTargetNotIncludingScale,SocketName);
		UE_LOG(LogTemp,Warning,TEXT("SpawnAndAttachLasers AttachToComponent"));

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this,Laser,SocketName]()
		{
			if(!Laser) return;

			// 디태치
			//Laser->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			auto* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
			if(!MyGameInstance) return;
			//MyGameInstance->Laser=false;
		//	Laser->Destroy();
		},5.0f,false);
	}
}