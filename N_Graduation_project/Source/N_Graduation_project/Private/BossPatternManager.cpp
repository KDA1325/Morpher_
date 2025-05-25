#include "BossPatternManager.h"
#include "TimerManager.h"
#include "ManaCoreCrystal.h"
#include "Kismet/GameplayStatics.h"
#include "N_Graduation_project/N_Graduation_projectCharacter.h"
ABossPatternManager::ABossPatternManager()
{
	PrimaryActorTick.bCanEverTick = true;

	ThunderCount=5;
}

void ABossPatternManager::BeginPlay()
{
	Super::BeginPlay();


}

void ABossPatternManager::SpawnThunder()
{
	AN_Graduation_projectCharacter* Player = Cast<AN_Graduation_projectCharacter>(UGameplayStatics::GetPlayerCharacter(this,0));
	if(!Player) return;

	FVector PlayerLocation = Player->GetActorLocation();


	FRotator SpawnRotation = FRotator::ZeroRotator;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	GetWorld()->SpawnActor<AActor>(ThunderBPClass,PlayerLocation,SpawnRotation,SpawnParams);
	UE_LOG(LogTemp,Warning,TEXT("Thunder %d번째 소환"),ThunderSpawnCount + 1);

	ThunderSpawnCount++;
	if(ThunderSpawnCount >= ThunderCount)
	{
		GetWorldTimerManager().ClearTimer(ThunderTimerHandle);
		ThunderSpawnCount = 0;  // 초기화 해주면 다음에도 다시 사용 가능
	}
}

void ABossPatternManager::Thunder(){
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

}