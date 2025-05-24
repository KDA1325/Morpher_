#include "BossPatternManager.h"
#include "TimerManager.h"
#include "ManaCoreCrystal.h"
#include "Kismet/GameplayStatics.h"
#include "N_Graduation_project/N_Graduation_projectCharacter.h"
#include "MyGameInstance.h"

ABossPatternManager::ABossPatternManager()
{
	PrimaryActorTick.bCanEverTick = true;
	SphereComponent = nullptr;
	ThunderCount=5;

}

void ABossPatternManager::BeginPlay()
{
	Super::BeginPlay();

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
	if(BossActor)
	{
		FVector BossHeadLocation = BossActor->GetActorLocation() + FVector(0,0,BossHeadHeightOffset); // 머리 위 위치 조절
		GetWorld()->SpawnActor<AActor>(ThunderBPClass,BossHeadLocation,SpawnRotation,SpawnParams);
		UE_LOG(LogTemp,Warning,TEXT("보스 머리 위 번개 생성"));
	}
	ThunderSpawnCount++;

	if(ThunderSpawnCount >= ThunderCount)
	{
		GetWorldTimerManager().ClearTimer(ThunderTimerHandle);
		ThunderSpawnCount = 0;  // 초기화 
		MyGameInstance->Thunder = false;

	}

}
 
void ABossPatternManager::Thunder(){
	UE_LOG(LogTemp,Warning,TEXT("Thunder 성공"));
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

		// Barrel인 경우 연쇄 폭발 처리
		if(Actor->ActorHasTag(FName("Player")))
		{
			// 데미지 처리
			AController* InstigatorController = GetInstigatorController();
			UGameplayStatics::ApplyDamage(Actor,ThunderDamage,InstigatorController,this,nullptr);
		}
	}

}
