
#include "Tester.h"
#include "Engine/World.h"
#include "Components/WidgetComponent.h"
#include "EntityWidget.h"
#include "Components/BoxComponent.h"  // UBoxComponent
#include "Components/ArrowComponent.h"  // UArrowComponent
#include "MyMonsterStatComponent.h"

// Sets default values
ATester::ATester()
{
	CurrentHP = 100.0f;
	PrimaryActorTick.bCanEverTick = true;

	// UStaticMeshComponent을 생성자에서 CreateDefaultSubobject로 생성
	// 생성자에서 미리 생성해줘야 nullptr 오류가 뜨지 않음 
	m_pMeshCom = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));

	// RootComponent 설정 -> 안 하면 m_pMeshCom nullptr 오류 발생
	if (!m_pMeshCom)
	{
		m_pMeshCom = NewObject<USkeletalMeshComponent>(this, TEXT("MeshComponent"));
		m_pMeshCom->RegisterComponent();
		RootComponent = m_pMeshCom;
	}
	MyMonsterStatComponent = CreateDefaultSubobject<UMyMonsterStatComponent>(TEXT("MyMonsterStatComponent"));
	EntityGroupID = TEXT("DefaultUniqueID");
}


// Called when the game starts or when spawned
void ATester::BeginPlay()
{
	Super::BeginPlay();
	if (!m_pMeshCom)
	{
		m_pMeshCom = NewObject<USkeletalMeshComponent>(this, TEXT("MeshComponent"));
		m_pMeshCom->RegisterComponent();
		RootComponent = m_pMeshCom;
	}
//	UE_LOG(LogTemp, Warning, TEXT("banana BeginPlay: EntityGroupID BEFORE UpdateEntityData: %s"), *EntityGroupID);
	SpawnEntityPreset();
	UpdateEntityData();
	// EntityData 값 확인용 로그 추가
//	UE_LOG(LogTemp, Warning, TEXT("banana BeginPlay: EntityGroupID %s, HP: %f"), *EntityGroupID, EntityData.HP);

}

void ATester::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATester::UpdateEntityData()
{
	static int32 CallCount = 0;
	CallCount++;

	// 데이터 갱신 시, HP 값이 올바르게 세팅되는지 확인
	UABGameSingleton::Get().GetEntityDataByGroupID(EntityGroupID, EntityData);
	SetActorLabel(EntityData.EntityName);

	// HP 값이 0 이하로 설정되지 않도록 방어 코드 추가
	if (EntityData.HP <= 0)
	{
		EntityData.HP = 100.0f; // 기본 HP 값으로 초기화
	}

	CurrentHP = EntityData.HP;
	SetHP(EntityData.HP);
	moveSpeed = EntityData.MoveSpeed;
	SetPreset(EntityData.PresetReference);

	UE_LOG(LogABGameSingleton, Error, TEXT("banana CallCount: %d Entity Name: %s, HP: %f, Move Speed: %d"),
		CallCount, *EntityData.EntityName, CurrentHP, EntityData.MoveSpeed);
}

void ATester::SpawnEntityPreset()
{
	if (EntityPresetClass)
	{
		AActor* SpawnedEntityPreset = GetWorld()->SpawnActor<AActor>(EntityPresetClass, GetActorLocation(), GetActorRotation());

		if (SpawnedEntityPreset)
		{
			// UWidgetComponent가 null이면 수동으로 초기화
			UWidgetComponent* WidgetComponent = SpawnedEntityPreset->FindComponentByClass<UWidgetComponent>();

			if (!WidgetComponent)
			{
				// UWidgetComponent가 없으면 새로운 컴포넌트를 추가
				WidgetComponent = NewObject<UWidgetComponent>(SpawnedEntityPreset);
				if (WidgetComponent)
				{
					// WidgetComponent를 부모에 첨부하고 등록
					WidgetComponent->SetupAttachment(SpawnedEntityPreset->GetRootComponent()); // 부모 컴포넌트에 넣고
					WidgetComponent->RegisterComponent();  // 컴포넌트를 월드에
				}
			}

			// WidgetComponent가 제대로 초기화되었는지 확인
			//if (WidgetComponent)
			//{
			//	// 위젯을 가져와서 업데이트
			//	UUserWidget* UserWidget = WidgetComponent->GetWidget();
			//	if (UEntityWidget* MyEntityWidget = Cast<UEntityWidget>(UserWidget))
			//	{
			//		// 위젯에서 초기 정보를 갱신
			//		MyEntityWidget->UpdateHealthBar(EntityData.HP);
			//		MyEntityWidget->MaxHP = EntityData.HP;
			//		MyEntityWidget->ReceiveEntityName(FText::FromString(EntityData.EntityName));
			//		MyEntityWidget->ReceiveEntitySpeed(EntityData.MoveSpeed);
			//		CurrentHP= EntityData.HP;
			//	}
			//}
		}
	}
}

void ATester::SetMoveSpeed(int32 MoveSpeed)
{
	currentSpeed = MoveSpeed;
}

void ATester::SetPreset(FString PresetReference)
{
	currentPreset = PresetReference;

	// 프리셋 이름마다 메시 에셋 파일 할당
	if (currentPreset == "PCPreset.uasset")
	{
		// 언리얼 콘텐츠 브라우저에서 에셋 파일 우클릭 ->  !!레퍼런스!! 복사 -> 텍스트로 넣어주기

		// FObjectFinder는 반드시 생성자에서만 사용 가능, 함수 내에서 사용할 시 바로 크래시 발생
		//ConstructorHelpers::FObjectFinder<UStaticMesh> BodyMesh(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Props/SM_Chair.SM_Chair'"));
		/*if (BodyMesh.Succeeded())
		{

		}*/
		//m_pMeshCom->SetStaticMesh(BodyMesh.Object);
		//RootComponent = m_pMeshCom;

		//FString MeshPath = TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Props/SM_Chair.SM_Chair'");
		//UStaticMesh* LoadedMesh = LoadObject<UStaticMesh>(nullptr, *MeshPath);

		// 에디터 실행 시 문제없이 메시를 로드하기 위해 FSoftObjectPath를 사용해 비동기 로딩 
		FSoftObjectPath MeshPath(TEXT("/Game/Animation/Boar/Boar_idle_test3s_2.Boar_idle_test3s_2"));

		USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshPath.TryLoad());
		if (LoadedMesh)
		{
			// 스켈레탈 메시를 사용할 경우 SetSkeletalMesh() 사용
			m_pMeshCom->SetSkeletalMesh(LoadedMesh);
		}
	}

	if (currentPreset == "WildBoarPreset.uasset")
	{
		FSoftObjectPath MeshPath(TEXT("/Game/Animation/Boar/Boar_idle_test3s_2.Boar_idle_test3s_2"));

		USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshPath.TryLoad());

		if (LoadedMesh)
		{
			m_pMeshCom->SetSkeletalMesh(LoadedMesh);  // SkeletalMesh는 Skel_MeshCom을 사용
			//	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("I'm Here")));
		}
	}

	if (currentPreset == "InpermonPreset.uasset")
	{
		FSoftObjectPath MeshPath(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Props/SM_TableRound.SM_TableRound'"));

		USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshPath.TryLoad());
		if (LoadedMesh)
		{
			m_pMeshCom->SetSkeletalMesh(LoadedMesh);
		}
	}

	if (currentPreset == "FreezardPreset.uasset")
	{
		FSoftObjectPath MeshPath(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Props/SM_Statue.SM_Statue'"));

		USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshPath.TryLoad());
		if (LoadedMesh)
		{
			m_pMeshCom->SetSkeletalMesh(LoadedMesh);
		}
	}

	if (currentPreset == "StoneGolemPreset.uasset")
	{
		FSoftObjectPath MeshPath(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Props/SM_Lamp_Ceiling.SM_Lamp_Ceiling'"));

		USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshPath.TryLoad());
		if (LoadedMesh)
		{
			m_pMeshCom->SetSkeletalMesh(LoadedMesh);
		}
	}
}
float ATester::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("banana ATester::TakeDamage! Damage: %f"), DamageAmount);
	UE_LOG(LogTemp, Log, TEXT("banana (TakeDamage)CurrentHP: %f, DamageAmount: %f, CurrentHP - DamageAmount: %f"), CurrentHP, DamageAmount, CurrentHP - DamageAmount);

	if (CurrentHP > 0)
	{
		ApplyDamage(DamageAmount);
		WidgetUpdate();
		//UE_LOG(LogTemp, Log, TEXT("banana Tester Monster get currentHp: %f Damage: %f"), CurrentHP, DamageAmount);
		UE_LOG(LogTemp, Log, TEXT("banana (CurrentHP>0)CurrentHP: %f, DamageAmount: %f, CurrentHP - DamageAmount: %f"), CurrentHP, DamageAmount, CurrentHP - DamageAmount);

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Monster Dead!"));
		UE_LOG(LogTemp, Log, TEXT("banana Tester Monster get currentHp: %f Damage: %f"), CurrentHP, DamageAmount);

		//Destroy();
	}
	return DamageAmount;

}

void ATester::SetHP(float NewHP)
{
	CurrentHP = NewHP;
	UE_LOG(LogTemp, Warning, TEXT("SetHP NewHP: %f"), NewHP);
	/*if (NewHP <= 0)
	{
		CurrentHP = 0;
		UE_LOG(LogTemp, Log, TEXT("banana CurrentHP=0"));

	}
	else
	{
		CurrentHP = NewHP;
		UE_LOG(LogTemp, Log, TEXT("banana Monster CurrentHP: %f"), CurrentHP);
	}*/
}

void ATester::ApplyDamage(float DamageAmount)
{
	SetHP(CurrentHP - DamageAmount);

}

void ATester::WidgetUpdate()
{
	AActor* SpawnedEntityPreset = GetWorld()->SpawnActor<AActor>(EntityPresetClass, GetActorLocation(), GetActorRotation());

	// nullptr 체크 추가
	if (!SpawnedEntityPreset)
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnEntityPreset failed!"));
		return;
	}

	UWidgetComponent* WidgetComponent = SpawnedEntityPreset->FindComponentByClass<UWidgetComponent>();

	if (WidgetComponent)
	{
		UUserWidget* UserWidget = WidgetComponent->GetWidget();
		if (UEntityWidget* MyEntityWidget = Cast<UEntityWidget>(UserWidget))
		{
			MyEntityWidget->UpdateHealthBar(CurrentHP);
		}
	}
}
