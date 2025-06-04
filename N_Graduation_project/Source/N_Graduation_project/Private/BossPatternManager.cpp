#include "BossPatternManager.h"
#include "TimerManager.h"
#include "ManaCoreCrystal.h"
#include "Kismet/GameplayStatics.h"
#include "N_Graduation_project/N_Graduation_projectCharacter.h"
#include "MyGameInstance.h"
#include "BossCharacter.h"
#include "BossProjectile.h"
#include "HealCrystal.h"
#include "BossProjectile2.h"
#include "Components/AudioComponent.h"
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

	Spinning1SocketNames = {"spin1","spin3","spin5","spin7"};
	Spinning2SocketNames= {"spin2","spin4","spin6","spin8"};
	//static ConstructorHelpers::FClassFinder<AActor> SpinBPClassFinder(TEXT("/Game/Entity/Boss/Boss_Projectile"));
	//if(SpinBPClassFinder.Succeeded())
	//{
	//	SpinningBPClass = SpinBPClassFinder.Class;
	//}


	CrystalSocketNames = {"Crystal1","Crystal2","Crystal3"};
	static ConstructorHelpers::FClassFinder<AActor> BlackClassFinder(TEXT("/Game/Entity/Boss/BlackCrystal"));
	if(BlackClassFinder.Succeeded())
	{
		BlackCryBPClass = BlackClassFinder.Class;
	}	static ConstructorHelpers::FClassFinder<AActor> RedClassFinder(TEXT("/Game/Entity/Boss/RedCrystal"));
	if(RedClassFinder.Succeeded())
	{
		RedCryBPClass = RedClassFinder.Class;
	}
	static ConstructorHelpers::FClassFinder<AActor> BlueClassFinder(TEXT("/Game/Entity/Boss/BlueCrystal"));
	if(BlueClassFinder.Succeeded())
	{
		BlueCryBPClass = BlueClassFinder.Class;
	}
	LazerBeamSound = LoadObject<USoundWave>(nullptr,TEXT("SoundWave'/Game/Sounds/Battle/LazerBeam.LazerBeam'"));
	SpinProjectileSound =  LoadObject<USoundWave>(nullptr,TEXT("SoundWave'/Game/Sounds/Battle/BossProjectileSpawn.BossProjectileSpawn'"));
	PlaySpinProjectileSound =  LoadObject<USoundWave>(nullptr,TEXT("SoundWave'/Game/Sounds/Battle/SpinProjectile.SpinProjectile'"));

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
	
	ThunderSpawnCount++;

	if(ThunderSpawnCount >= ThunderCount)
	{
		UE_LOG(LogTemp,Warning,TEXT("ThunderSpawnCount"));

		GetWorldTimerManager().ClearTimer(ThunderTimerHandle);
		ThunderSpawnCount = 0;  // 초기화 
		MyGameInstance->Thunder = false;
		thunderOnce=false;

	}

}

void ABossPatternManager::Thunder(){
	//UE_LOG(LogTemp,Warning,TEXT(" 성공"));

	//if(thunderOnce==false){
	//	thunderOnce=true;
	//	auto* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	//	if(!MyGameInstance) return;
	//	UClass* LoadedClass = LoadObject<UClass>(nullptr,TEXT("/Game/Entity/Boss/Boss_Thunder.Boss_Thunder_C"));
	//	if(LoadedClass)
	//	{
	//		ThunderBPClass = LoadedClass;
	//		UE_LOG(LogTemp,Warning,TEXT("ThunderBPClass 동적 로드 성공"));

	//	} else
	//	{
	//		UE_LOG(LogTemp,Warning,TEXT("ThunderBPClass 로드 실패"));
	//	}

	//	GetWorldTimerManager().SetTimer(ThunderTimerHandle,this,&ABossPatternManager::SpawnThunder,1.0f,true);
	//	MyGameInstance->Thunder = true;
	//}
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

		UAudioComponent* LaserAudioComp = UGameplayStatics::SpawnSoundAttached(
		LazerBeamSound, //  SoundWave 직접 사용
		Laser->GetRootComponent(),
		NAME_None,
		FVector::ZeroVector,
		EAttachLocation::KeepRelativeOffset,
		true  // 
		);

		//FTimerHandle TimerHandle;
		//GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this,Laser,SocketName]()
		//{
		//	if(!Laser) return;

		//	// 디태치
		//	Laser->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		//	auto* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		//	if(!MyGameInstance) return;
		//	MyGameInstance->Laser=false;
		//	Laser->Destroy();
		//},6.0f,false);

		FTimerHandle TimerHandle;
		TWeakObjectPtr<AActor> WeakLaser = Laser;
		TWeakObjectPtr<ABossPatternManager> WeakThis = this;

		GetWorld()->GetTimerManager().SetTimer(TimerHandle,[WeakLaser,WeakThis]()
		{
			if(!WeakThis.IsValid()) return; // this가 삭제된 상태면 무시
			if(!WeakLaser.IsValid()) return; // 레이저가 이미 삭제됨

			AActor* LaserActor = WeakLaser.Get();
			LaserActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

			if(UWorld* World = LaserActor->GetWorld())
			{
				if(UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(World)))
				{
					MyGameInstance->Laser = false;
				}
			}

			LaserActor->Destroy();

		},6.0f,false);
	}
}

void ABossPatternManager::SpinningBarrage()
{
	auto* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if(!MyGameInstance) return;

	ABossCharacter* BossChar = Cast<ABossCharacter>(BossActor);
	if(!BossChar)
	{
		UE_LOG(LogTemp,Error,TEXT("SpinningBarrage: BossActor is not valid!"));
		return;
	}

	if(!BossChar->ProjectileClass)
	{
		UE_LOG(LogTemp,Error,TEXT("SpinningBarrage: ProjectileClass is not set!"));
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	MyGameInstance->Spin = true;

	// Spinning1 소켓에서 일반 회전 투사체 발사
	for(const FName& SocketName : Spinning1SocketNames)
	{
		FireProjectileAtSocket(SocketName,false);
	}

	// Spinning2 소켓에서 반대 방향 또는 다른 투사체 발사
	for(const FName& SocketName : Spinning2SocketNames)
	{
		FireProjectileAtSocket(SocketName,true);
	}
}
void ABossPatternManager::StartSpinningBarrageSequence(int num)
{
	if(!IsValid(this))
	{
		UE_LOG(LogTemp,Error,TEXT("ABossPatternManager is invalid"));
		return;
	}

	SpinningBarrageCount = num;
	UAudioComponent* SpinAudioComp = UGameplayStatics::SpawnSoundAttached(
			PlaySpinProjectileSound,
			this->GetRootComponent(), //  여기에 붙이기
			NAME_None,
			FVector::ZeroVector,
			EAttachLocation::KeepRelativeOffset,
			true // 투사체가 파괴되면 사운드도 같이 꺼짐
	);
	GetWorld()->GetTimerManager().SetTimer(
		SpinningBarrageTimerHandle,
		this,
		&ABossPatternManager::SpinningBarrageTick,
		0.5f,
		true
	);
}

void ABossPatternManager::SpinningBarrageTick()
{
	auto* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if(!MyGameInstance) return;
	if(SpinningBarrageCount >= 20) //
	{
		GetWorld()->GetTimerManager().ClearTimer(SpinningBarrageTimerHandle);
		MyGameInstance->Spin=false;

		return;
	}

	SpinningBarrage();
	SpinningBarrageCount++;
}

void ABossPatternManager::StartSpinningBarrageSequence2(int num)
{
	if(!IsValid(this))
	{
		UE_LOG(LogTemp,Error,TEXT("ABossPatternManager is invalid"));
		return;
	}

	SpinningBarrageCount = num;

	GetWorld()->GetTimerManager().SetTimer(
		SpinningBarrageTimerHandle2,
		this,
		&ABossPatternManager::SpinningBarrageTick2,
		0.5f,
		true
	);
}

void ABossPatternManager::SpinningBarrageTick2()
{
	auto* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if(!MyGameInstance) return;
	if(SpinningBarrageCount >= 25) //
	{
		GetWorld()->GetTimerManager().ClearTimer(SpinningBarrageTimerHandle2);
		MyGameInstance->Spin=false;

		return;
	}

	SpinningBarrage2();
	SpinningBarrageCount++;
}

void ABossPatternManager::SpinningBarrage2()
{
	auto* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if(!MyGameInstance) return;

	ABossCharacter* BossChar = Cast<ABossCharacter>(BossActor);
	if(!BossChar)
	{
		UE_LOG(LogTemp,Error,TEXT("SpinningBarrage: BossActor is not valid!"));
		return;
	}

	if(!BossChar->ProjectileClass)
	{
		UE_LOG(LogTemp,Error,TEXT("SpinningBarrage: ProjectileClass is not set!"));
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	MyGameInstance->Spin = true;

	// Spinning1 소켓에서 일반 회전 투사체 발사
	for(const FName& SocketName : Spinning1SocketNames)
	{
		FireProjectileAtSocket(SocketName,false);
	}

	// Spinning2 소켓에서 반대 방향 또는 다른 투사체 발사
	for(const FName& SocketName : Spinning2SocketNames)
	{
		FireProjectileAtSocket(SocketName,true);
	}
}




void ABossPatternManager::HealCrystal()
{
	auto* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if(!MyGameInstance) return;

	ABossCharacter* BossChar = Cast<ABossCharacter>(BossActor);
	if(!BossChar || !BossChar->GetMesh()) return;

	for(int32 i = 0; i < CrystalSocketNames.Num(); ++i)
	{
		TSubclassOf<AActor> SelectedClass = nullptr;

		if(i == 0 && BlackCryBPClass) SelectedClass = BlackCryBPClass;
		else if(i == 1 && RedCryBPClass) SelectedClass = RedCryBPClass;
		else if(i == 2 && BlueCryBPClass) SelectedClass = BlueCryBPClass;

		if(SelectedClass)
		{
			FName SocketName = CrystalSocketNames[i];
			FTransform SocketTransform = BossChar->GetMesh()->GetSocketTransform(SocketName);
			SocketTransform.AddToTranslation(FVector(0,0,50));

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			AActor* SpawnedCrystal = GetWorld()->SpawnActor<AActor>(SelectedClass,SocketTransform,SpawnParams);
			if(!SpawnedCrystal)
			{
				UE_LOG(LogTemp,Error,TEXT("HealCrystal: SpawnActor 실패! %d"),i);
				continue;
			}

			UE_LOG(LogTemp,Warning,TEXT("HealCrystal: 크리스탈 스폰 성공: %s"),*SpawnedCrystal->GetName());
			MyGameInstance->BossHeal=true;
			SpawnedCrystal->AttachToComponent(
				BossChar->GetMesh(),
				FAttachmentTransformRules::SnapToTargetNotIncludingScale,
				SocketName
			);


			AHealCrystal* HealCrystal = Cast<AHealCrystal>(SpawnedCrystal);
			if(HealCrystal)
			{
				HealCrystal->BossActor = BossChar;
				HealCrystal->StartCrystal(15);             // 각 크리스탈 별로 타이머 시작
			}
		}

	}
}

void ABossPatternManager::Meteor()
{

	auto* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if(!MyGameInstance) return;

	// 동적 로드
	UClass* LoadedClass = LoadObject<UClass>(nullptr,TEXT("/Game/Entity/Boss/Boss_Meteor.Boss_Meteor_C"));
	if(LoadedClass)
	{
		MeteorBPClass = LoadedClass;
		UE_LOG(LogTemp,Warning,TEXT("MeteorBPClass 로드 성공"));
	} else
	{
		UE_LOG(LogTemp,Error,TEXT("MeteorBPClass 로드 실패"));
		return;
	}

	MeteorSpawnCount = 0;
	GetWorldTimerManager().SetTimer(
		MeteorTimerHandle,
		this,
		&ABossPatternManager::SpawnMeteorBatch,
		0.1f,  // 초당 1회
		true
	);
}

void ABossPatternManager::SpawnMeteorBatch()
{
	if(!MeteorBPClass) return;

	for(int i = 0; i < MeteorsPerSecond; ++i)
	{
		if(MeteorSpawnCount >= MeteorTotalCount)
			break;

		FVector SpawnLocation = GetRandomMeteorLocation();
		FRotator SpawnRotation = FRotator::ZeroRotator;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		GetWorld()->SpawnActor<AActor>(MeteorBPClass,SpawnLocation,SpawnRotation,SpawnParams);
		UE_LOG(LogTemp,Warning,TEXT("Meteor %d번째 소환"),MeteorSpawnCount + 1);

		MeteorSpawnCount++;
	}

	if(MeteorSpawnCount >= MeteorTotalCount)
	{
		GetWorldTimerManager().ClearTimer(MeteorTimerHandle);
		MeteorSpawnCount = 0;
	}
}

FVector ABossPatternManager::GetRandomMeteorLocation()
{
	// 맵 또는 보스 기준 랜덤 위치 설정 (원하는 범위 조정 가능)
	FVector Origin(0.0f,0.0f,0); // 높은 Z 위치에서 낙하
	float RangeXY = 3000.0f;
	FVector RandomOffset = FVector(
		FMath::FRandRange(-RangeXY,RangeXY),
		FMath::FRandRange(-RangeXY,RangeXY),
		0.0f
	);
	return Origin + RandomOffset;
}
void ABossPatternManager::ApplyMeteorDamage(){
	SphereComponent = FindComponentByClass<USphereComponent>();
	if(SphereComponent)
	{
		SphereComponent->SetGenerateOverlapEvents(true);
		UE_LOG(LogTemp,Log,TEXT("Meteor: SphereComponent 세팅 완료"));
	} else
	{
		UE_LOG(LogTemp,Error,TEXT("Meteor: SphereComponent가 NULL입니다! BP에서 설정되었는지 확인하세요"));
	}

	TArray<AActor*> OverlappingActors;
	SphereComponent->GetOverlappingActors(OverlappingActors);

	for(AActor* Actor : OverlappingActors)
	{
		if(!Actor || Actor == this)
			continue;


		if(Actor->ActorHasTag(FName("Player")))
		{
			// 데미지 처리
			AController* InstigatorController = GetInstigatorController();
			UGameplayStatics::ApplyDamage(Actor,40,InstigatorController,this,nullptr);
		}
	}
}

void ABossPatternManager::FireProjectileAtSocket(const FName& SocketName,bool bUseSkillData)
{
	ABossCharacter* BossChar = Cast<ABossCharacter>(BossActor);
	if(!BossChar || !BossChar->ProjectileClass) return;

	FTransform SocketTransform = BossChar->GetMesh()->GetSocketTransform(SocketName);
	FVector SpawnLocation = SocketTransform.GetLocation();
	FVector FireDirection = SocketTransform.GetRotation().GetForwardVector();
	FRotator SpawnRotation = FireDirection.Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	ABossProjectile* SpawnedProjectile = GetWorld()->SpawnActor<ABossProjectile>(
		BossChar->ProjectileClass,SpawnLocation,SpawnRotation,SpawnParams);

	if(SpawnedProjectile)
	{
		if(bUseSkillData)
		{
			FSkillData SkillData;
			TArray<FSkillEffectData> EffectDataArray;
			if(UABGameSingleton::Get().GetSkillDataBySkillID("Skill_ThrowRock",SkillData) &&
				UABGameSingleton::Get().GetSkillEffectDataBySkillID("Skill_ThrowRock",EffectDataArray))
			{
				SpawnedProjectile->InitProjectileBySkillData(ProjectileSpeed,ApplyDamageAmount);
			}
		} else
		{
			SpawnedProjectile->InitProjectileBySkillData(ProjectileSpeed,ApplyDamageAmount);
		}

		SpawnedProjectile->FireInDirection(FireDirection);
	}
}

