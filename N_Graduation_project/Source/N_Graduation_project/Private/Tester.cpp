// Fill out your copyright notice in the Description page of Project Settings.


#include "Tester.h"
#include "Engine/World.h"
#include "Components/WidgetComponent.h"
#include "EntityWidget.h"

// Sets default values
ATester::ATester()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// UStaticMeshComponent을 생성자에서 CreateDefaultSubobject로 생성
	// 생성자에서 미리 생성해줘야 nullptr 오류가 뜨지 않음 
	m_pMeshCom = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));

	// RootComponent 설정 -> 안 하면 m_pMeshCom nullptr 오류 발생
	if (!m_pMeshCom)
	{
		m_pMeshCom = NewObject<UStaticMeshComponent>(this, TEXT("MeshComponent"));
		m_pMeshCom->RegisterComponent();
		RootComponent = m_pMeshCom;
	}
}

// Called when the game starts or when spawned
void ATester::BeginPlay()
{
	Super::BeginPlay();

	if (!m_pMeshCom)
	{
		m_pMeshCom = NewObject<UStaticMeshComponent>(this, TEXT("MeshComponent"));
		m_pMeshCom->RegisterComponent();
		RootComponent = m_pMeshCom;
	}

	UpdateEntityData();
	SpawnEntityPreset();
}

// Called every frame
void ATester::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATester::UpdateEntityData()
{
	if (UABGameSingleton::Get().GetEntityDataByGroupID(EntityGroupID, EntityData))
	{
		SetActorLabel(EntityData.EntityName);
		SetMaxHp(EntityData.HP);
		SetMoveSpeed(EntityData.MoveSpeed);
		SetPreset(EntityData.PresetReference);

		UE_LOG(LogABGameSingleton, Error, TEXT("Entity Name: %s, HP: %d, Move Speed: %d"),
			*EntityData.EntityName, EntityData.HP, EntityData.MoveSpeed);

		//SpawnEntityPreset();
	}

}
/*void ATester::SpawnEntityPreset()
{
	//null오류 잦음 null 확인 코드 필요(내일꼭하기)
	//두번째 실행부터 문제생김
	if (EntityPresetClass)
	{
		AActor* SpawnedEntityPreset = GetWorld()->SpawnActor<AActor>(EntityPresetClass, GetActorLocation(), GetActorRotation());
		// EntityPreset에서 WidgetComponent에 접근
		if (UWidgetComponent* WidgetComponent = SpawnedEntityPreset->FindComponentByClass<UWidgetComponent>())
		{
			if (UEntityWidget* MyEntityWidget = Cast<UEntityWidget>(WidgetComponent->GetWidget()))
			{
				MyEntityWidget->UpdateHealthBar(EntityData.HP);
				MyEntityWidget->SetEntityName(FText::FromString(EntityData.EntityName));
			}
		}
	}
}
*/

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
					WidgetComponent->SetupAttachment(SpawnedEntityPreset->GetRootComponent()); // 부모 컴포넌트에 첨부
					WidgetComponent->RegisterComponent();  // 컴포넌트를 월드에 등록
				}
			}

			// WidgetComponent가 제대로 초기화되었는지 확인
			if (WidgetComponent)
			{
				// 위젯을 가져와서 업데이트
				UUserWidget* UserWidget = WidgetComponent->GetWidget();
				if (UEntityWidget* MyEntityWidget = Cast<UEntityWidget>(UserWidget))
				{
					// 위젯에서 정보를 갱신
					MyEntityWidget->UpdateHealthBar(EntityData.HP);
					MyEntityWidget->SetEntityName(FText::FromString(EntityData.EntityName));
				}
				else
				{
					// UEntityWidget으로 캐스팅 실패 시, 로그를 사용하지 않고 처리를 진행.
				}
			}
			else
			{
				// WidgetComponent가 null인 경우
			}
		}
		else
		{
			// Spawn된 EntityPreset이 유효하지 않음 (null인 경우)
		}
	}
	else
	{
		// EntityPresetClass가 null인 경우
	}
}



void ATester::SetMaxHp(int32 MaxHp)
{
	currentHp = MaxHp;
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
		FSoftObjectPath MeshPath(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Props/SM_Chair.SM_Chair'"));

		UStaticMesh* LoadedMesh = Cast<UStaticMesh>(MeshPath.TryLoad());
		if (LoadedMesh)
		{
			// 스켈레탈 메시를 사용할 경우 SetSkeletalMesh() 사용
			m_pMeshCom->SetStaticMesh(LoadedMesh);
		}
	}


	if (currentPreset == "WildBoarPreset.uasset")
	{
		FSoftObjectPath MeshPath(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Props/SM_Bush.SM_Bush'"));

		UStaticMesh* LoadedMesh = Cast<UStaticMesh>(MeshPath.TryLoad());
		if (LoadedMesh)
		{
			m_pMeshCom->SetStaticMesh(LoadedMesh);
		}
	}

	if (currentPreset == "InpermonPreset.uasset")
	{
		FSoftObjectPath MeshPath(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Props/SM_TableRound.SM_TableRound'"));

		UStaticMesh* LoadedMesh = Cast<UStaticMesh>(MeshPath.TryLoad());
		if (LoadedMesh)
		{
			m_pMeshCom->SetStaticMesh(LoadedMesh);
		}
	}

	if (currentPreset == "FreezardPreset.uasset")
	{
		FSoftObjectPath MeshPath(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Props/SM_Statue.SM_Statue'"));

		UStaticMesh* LoadedMesh = Cast<UStaticMesh>(MeshPath.TryLoad());
		if (LoadedMesh)
		{
			m_pMeshCom->SetStaticMesh(LoadedMesh);
		}
	}

	if (currentPreset == "StoneGolemPreset.uasset")
	{
		FSoftObjectPath MeshPath(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Props/SM_Lamp_Ceiling.SM_Lamp_Ceiling'"));

		UStaticMesh* LoadedMesh = Cast<UStaticMesh>(MeshPath.TryLoad());
		if (LoadedMesh)
		{
			m_pMeshCom->SetStaticMesh(LoadedMesh);
		}
	}
}


