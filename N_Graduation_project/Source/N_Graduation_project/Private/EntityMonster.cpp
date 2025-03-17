// Fill out your copyright notice in the Description page of Project Settings.


#include "EntityMonster.h"
#include "Engine/World.h"
// #include "Components/WidgetComponent.h"
#include "EntityWidget.h"

// Sets default values
AEntityMonster::AEntityMonster()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 위젯 컴포넌트 초기화
	//WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	//if (WidgetComponent)
	//{
	//	// RootComponent에 붙이거나 Mesh에 붙이기
	//	WidgetComponent->SetupAttachment(RootComponent);
	//	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	//	WidgetComponent->SetVisibility(false);  // 처음엔 안 보이게
	//}
	// GetMesh()로 기본 스켈레탈 메시 컴포넌트를 가져옴
	//GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f));
	//GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	// UStaticMeshComponent을 생성자에서 CreateDefaultSubobject로 생성
	// 생성자에서 미리 생성해줘야 nullptr 오류가 뜨지 않음 
	//m_pMeshCom = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	//m_pMeshCom = GetMesh();

	//// RootComponent 설정 -> 안 하면 m_pMeshCom nullptr 오류 발생
	//if (!m_pMeshCom)
	//{
	//	//m_pMeshCom = NewObject<USkeletalMeshComponent>(this, TEXT("MeshComponent"));
	//	////m_pMeshCom = GetMesh();	
	//	//m_pMeshCom->RegisterComponent();
	//	//RootComponent = m_pMeshCom;
	//	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("failed to get mesh")));
	//	return;
	//}

	//RootComponent = m_pMeshCom;
}

// Called when the game starts or when spawned
void AEntityMonster::BeginPlay()
{
	Super::BeginPlay();
	isSpawned = false;
	//
	//if (!m_pMeshCom)
	//{
	//	/*m_pMeshCom = NewObject<USkeletalMeshComponent>(this, TEXT("MeshComponent"));
	//	m_pMeshCom->RegisterComponent();
	//	RootComponent = m_pMeshCom;*/

	//	m_pMeshCom = GetMesh();

	//	if (!m_pMeshCom)
	//	{
	//		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("failed to get mesh")));
	//		return;
	//	}
	//}

	UpdateEntityData();
}

// Called every frame
void AEntityMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEntityMonster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEntityMonster::UpdateEntityData()
{
	if (UABGameSingleton::Get().GetEntityDataByGroupID(EntityGroupID, EntityData))
	{
		SetActorLabel(EntityData.EntityName);
		SetMaxHp(EntityData.HP);
		SetMoveSpeed(EntityData.MoveSpeed);
		//SetPreset(EntityData.PresetReference);

		UE_LOG(LogABGameSingleton, Error, TEXT("Entity Name: %s, HP: %d, Move Speed: %d"),
			*EntityData.EntityName, EntityData.HP, EntityData.MoveSpeed);
	}

}

void AEntityMonster::SpawnEntityPreset()
{
	// Entity별 메시 세팅 
	SetPreset(EntityData.PresetReference);

	isSpawned = true;

	if (EntityPresetClass)
	{
		AActor* SpawnedEntityPreset = GetWorld()->SpawnActor<AActor>(EntityPresetClass, GetActorLocation(), GetActorRotation());

		//if (EntityPresetClass)
		//{
		//	AActor* SpawnedEntityPreset = GetWorld()->SpawnActor<AActor>(EntityPresetClass, GetActorLocation(), GetActorRotation());

		//	if (SpawnedEntityPreset && WidgetComponent)
		//	{
		//		WidgetComponent->SetVisibility(true);       // 위젯 활성화
		//		WidgetComponent->SetComponentTickEnabled(true); // Tick 활성화
		//	}
		//}

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
			if (WidgetComponent)
			{
				// 위젯을 가져와서 업데이트
				UUserWidget* UserWidget = WidgetComponent->GetWidget();
				if (UEntityWidget* MyEntityWidget = Cast<UEntityWidget>(UserWidget))
				{
					// 위젯에서 정보를 갱신
					MyEntityWidget->UpdateHealthBar(EntityData.HP);
					// 이름과 속도 값을 EntityWidget에 전달
					MyEntityWidget->ReceiveEntityName(FText::FromString(EntityData.EntityName));
					MyEntityWidget->ReceiveEntitySpeed(EntityData.MoveSpeed);
				}
			}
		}
	}
}



void AEntityMonster::SetMaxHp(int32 MaxHp)
{
	currentHp = MaxHp;
}

void AEntityMonster::SetMoveSpeed(int32 MoveSpeed)
{
	currentSpeed = MoveSpeed;
}

void AEntityMonster::SetPreset(FString PresetReference)
{
	currentPreset = PresetReference;

	// 프리셋 이름마다 메시 에셋 파일 할당
	if (currentPreset == "PCPreset.uasset")
	{
		// 언리얼 콘텐츠 브라우저에서 에셋 파일 우클릭 ->  !!레퍼런스!! 복사 -> 텍스트로 넣어주기
		// 에디터 실행 시 문제없이 메시를 로드하기 위해 FSoftObjectPath를 사용해 비동기 로딩 
		FSoftObjectPath MeshPath(TEXT("/Script/Engine.SkeletalMesh'/Game/Animation/Boar/Boar_idle_test3s_3.Boar_idle_test3s_3'"));
		FSoftObjectPath AnimPath(TEXT("/Script/Engine.AnimSequence'/Game/Animation/Boar/Boar_idle_test3s_2_Anim.Boar_idle_test3s_2_Anim'"));

		USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshPath.TryLoad());
		UAnimSequence* LoadedAnim = Cast<UAnimSequence>(AnimPath.TryLoad());

		if (LoadedMesh)
		{
			// 스켈레탈 메시를 사용할 경우 SetSkeletalMesh() 사용
			GetMesh()->SetSkeletalMesh(LoadedMesh);
			//m_pMeshCom->SetSkeletalMesh(LoadedMesh);
			if (LoadedAnim)
			{
				GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
				GetMesh()->SetAnimation(LoadedAnim);
				GetMesh()->Play(true);

				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Animation Load")));
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Animation Load Failed ")));
			}
		}
	}

	if (currentPreset == "WildBoarPreset.uasset")
	{
		FSoftObjectPath MeshPath(TEXT("/Game/Animation/Boar/Boar_idle_test3s_2.Boar_idle_test3s_2"));
		FSoftObjectPath AnimPath(TEXT("/Script/Engine.AnimSequence'/Game/Animation/Boar/Boar_idle_test3s_2_Anim.Boar_idle_test3s_2_Anim'"));

		USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshPath.TryLoad());
		UAnimSequence* LoadedAnim = Cast<UAnimSequence>(AnimPath.TryLoad());

		if (LoadedMesh)
		{
			GetMesh()->SetSkeletalMesh(LoadedMesh);  // SkeletalMesh는 Skel_MeshCom을 사용
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Mesh Load")));

			if (LoadedAnim)
			{
				GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
				GetMesh()->SetAnimation(LoadedAnim);
				GetMesh()->Play(true);

				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Animation Load")));
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Animation Load Failed ")));
			}
			//	
			//	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("I'm Here")));
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Mesh Load Failed ")));
		}
	}

	if (currentPreset == "InpermonPreset.uasset")
	{
		FSoftObjectPath MeshPath(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Props/SM_TableRound.SM_TableRound'"));

		USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshPath.TryLoad());
		if (LoadedMesh)
		{
			GetMesh()->SetSkeletalMesh(LoadedMesh);
		}
	}

	if (currentPreset == "FreezardPreset.uasset")
	{
		FSoftObjectPath MeshPath(TEXT("/Script/Engine.SkeletalMesh'/Game/Animation/Boar/Boar_idle_test3s_4.Boar_idle_test3s_4'"));
		FSoftObjectPath AnimPath(TEXT("/Script/Engine.AnimSequence'/Game/Animation/Boar/Boar_idle_test3s_2_Anim.Boar_idle_test3s_2_Anim'"));

		USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshPath.TryLoad());
		UAnimSequence* LoadedAnim = Cast<UAnimSequence>(AnimPath.TryLoad());

		if (LoadedMesh)
		{
			GetMesh()->SetSkeletalMesh(LoadedMesh);
			if (LoadedAnim)
			{
				GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
				GetMesh()->SetAnimation(LoadedAnim);
				GetMesh()->Play(true);

				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Animation Load")));
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Animation Load Failed ")));
			}
		}
	}

	if (currentPreset == "StoneGolemPreset.uasset")
	{
		FSoftObjectPath MeshPath(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Props/SM_Lamp_Ceiling.SM_Lamp_Ceiling'"));

		USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshPath.TryLoad());
		if (LoadedMesh)
		{
			GetMesh()->SetSkeletalMesh(LoadedMesh);
		}
	}
}