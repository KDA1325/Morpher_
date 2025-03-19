// Fill out your copyright notice in the Description page of Project Settings.


#include "EntityCharacter.h"
#include "MyAIController.h"
#include "EntityWidget.h"

// Sets default values
AEntityCharacter::AEntityCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AMyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AEntityCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//UpdateEntityData();
}

// Called every frame
void AEntityCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEntityCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEntityCharacter::SetMaxHp(int32 MaxHp)
{
	currentHp = MaxHp;
}

void AEntityCharacter::SetMoveSpeed(int32 MoveSpeed)
{
	currentSpeed = MoveSpeed;
}

//void AEntityCharacter::GetEntityGroupID(const FString& EntityGroupID)
//{
//	currentEntityGroupID = EntityGroupID;
//
//	UpdateEntityData();
//}
//
//void AEntityCharacter::UpdateEntityData()
//{
//	if (UABGameSingleton::Get().GetEntityDataByGroupID(currentEntityGroupID, EntityData))
//	{
//		SetActorLabel(EntityData.EntityName);
//		SetMaxHp(EntityData.HP);
//		SetMoveSpeed(EntityData.MoveSpeed);
//
//		UE_LOG(LogABGameSingleton, Error, TEXT("Entity Name: %s, HP: %d, Move Speed: %d"),
//			*EntityData.EntityName, EntityData.HP, EntityData.MoveSpeed);
//	}
//}

//void AEntityCharacter::SpawnEntityCharacter()
//{
//	// Entity별 메시 세팅 
//	SetPreset(EntityData.PresetReference);
//
//
//	if (EntitySpawnerClass)
//	{
//		AEntityCharacter* SpawnedEntityCharacter = Cast<AEntityCharacter>(GetWorld()->SpawnActor<AActor>(EntitySpawnerClass, GetActorLocation(), GetActorRotation()));
//
//		if (SpawnedEntityCharacter)
//		{
//			// 위젯 설정 및 업데이트
//			UWidgetComponent* WidgetComponent = SpawnedEntityCharacter->FindComponentByClass<UWidgetComponent>();
//			if (WidgetComponent)
//			{
//				UUserWidget* UserWidget = WidgetComponent->GetWidget();
//				if (UEntityWidget* MyEntityWidget = Cast<UEntityWidget>(UserWidget))
//				{
//					MyEntityWidget->UpdateHealthBar(currentHp);
//					MyEntityWidget->ReceiveEntityName(FText::FromString(EntityData.EntityName));
//					MyEntityWidget->ReceiveEntitySpeed(currentSpeed);
//					UE_LOG(LogTemp, Warning, TEXT("Widget updated successfully"));
//				}
//			}
//			else
//			{
//				UE_LOG(LogTemp, Error, TEXT("WidgetComponent not found"));
//			}
//		}
//		else
//		{
//			UE_LOG(LogTemp, Error, TEXT("SpawnEntityPreset failed"));
//		}
//	}
//}

void AEntityCharacter::InitializeEntity(FABEntityData& InEntityData)
{
	//currentEntityGroupID = InEntityData.EntityGroupID;

	SetActorLabel(InEntityData.EntityName);
	SetMaxHp(InEntityData.HP);
	SetMoveSpeed(InEntityData.MoveSpeed);
	SetPreset(InEntityData.PresetReference);

	// 로깅
	UE_LOG(LogTemp, Warning, TEXT("Initialized Entity with Name: %s, HP: %d, Move Speed: %d"),
		*InEntityData.EntityName, InEntityData.HP, InEntityData.MoveSpeed);

	// 메시와 애니메이션 세팅
	SetPreset(InEntityData.PresetReference);

}

void AEntityCharacter::SetPreset(FString PresetReference)
{
	currentPreset = PresetReference;
	// 프리셋 이름과 경로를 매핑하는 맵 선언
	TMap<FString, FSoftObjectPath> SkeletalMeshPaths = {
		{"PCPreset.uasset", FSoftObjectPath(TEXT("/Game/Animation/Boar/Boar_idle_test3s_3.Boar_idle_test3s_3"))},
		{"WildBoarPreset.uasset", FSoftObjectPath(TEXT("/Game/Animation/Boar/Boar_idle_test3s_2.Boar_idle_test3s_2"))},
		{"FreezardPreset.uasset", FSoftObjectPath(TEXT("/Game/Animation/Boar/Boar_idle_test3s_4.Boar_idle_test3s_4"))}
	};

	TMap<FString, FSoftObjectPath> AnimPaths = {
		{"PCPreset.uasset", FSoftObjectPath(TEXT("/Game/Animation/Boar/Boar_idle_test3s_2_Anim.Boar_idle_test3s_2_Anim"))},
		{"WildBoarPreset.uasset", FSoftObjectPath(TEXT("/Game/Animation/Boar/Boar_idle_test3s_2_Anim.Boar_idle_test3s_2_Anim"))},
		{"FreezardPreset.uasset", FSoftObjectPath(TEXT("/Game/Animation/Boar/Boar_idle_test3s_2_Anim.Boar_idle_test3s_2_Anim"))}
	};

	// Skeletal Mesh 처리
	if (SkeletalMeshPaths.Contains(currentPreset))
	{
		FSoftObjectPath MeshPath = SkeletalMeshPaths[currentPreset];
		USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshPath.TryLoad());
		if (LoadedMesh)
		{
			GetMesh()->SetSkeletalMesh(LoadedMesh);
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("Skeletal Mesh Loaded Successfully"));

			// 애니메이션 처리
			if (AnimPaths.Contains(currentPreset))
			{
				FSoftObjectPath AnimPath = AnimPaths[currentPreset];
				UAnimSequence* LoadedAnim = Cast<UAnimSequence>(AnimPath.TryLoad());
				if (LoadedAnim)
				{
					GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
					GetMesh()->SetAnimation(LoadedAnim);
					GetMesh()->Play(true);
					GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("Animation Loaded Successfully"));
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Animation Load Failed: %s"), *AnimPath.ToString());
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Skeletal Mesh Load Failed: %s"), *MeshPath.ToString());
		}
	}

	//// 프리셋 이름마다 메시 에셋 파일 할당
	//if (currentPreset == "PCPreset.uasset")
	//{
	//	// 언리얼 콘텐츠 브라우저에서 에셋 파일 우클릭 ->  !!레퍼런스!! 복사 -> 텍스트로 넣어주기
	//	// 에디터 실행 시 문제없이 메시를 로드하기 위해 FSoftObjectPath를 사용해 비동기 로딩 
	//	FSoftObjectPath MeshPath(TEXT("/Script/Engine.SkeletalMesh'/Game/Animation/Boar/Boar_idle_test3s_3.Boar_idle_test3s_3'"));
	//	FSoftObjectPath AnimPath(TEXT("/Script/Engine.AnimSequence'/Game/Animation/Boar/Boar_idle_test3s_2_Anim.Boar_idle_test3s_2_Anim'"));

	//	USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshPath.TryLoad());
	//	UAnimSequence* LoadedAnim = Cast<UAnimSequence>(AnimPath.TryLoad());

	//	if (LoadedMesh)
	//	{
	//		GetMesh()->SetSkeletalMesh(LoadedMesh);

	//		if (LoadedAnim)
	//		{
	//			GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	//			GetMesh()->SetAnimation(LoadedAnim);
	//			GetMesh()->Play(true);

	//			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Animation Load")));
	//		}
	//		else
	//		{
	//			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Animation Load Failed ")));
	//		}
	//	}
	//}

	//if (currentPreset == "WildBoarPreset.uasset")
	//{
	//	FSoftObjectPath MeshPath(TEXT("/Game/Animation/Boar/Boar_idle_test3s_2.Boar_idle_test3s_2"));
	//	FSoftObjectPath AnimPath(TEXT("/Script/Engine.AnimSequence'/Game/Animation/Boar/Boar_idle_test3s_2_Anim.Boar_idle_test3s_2_Anim'"));

	//	USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshPath.TryLoad());
	//	UAnimSequence* LoadedAnim = Cast<UAnimSequence>(AnimPath.TryLoad());

	//	if (LoadedMesh)
	//	{
	//		GetMesh()->SetSkeletalMesh(LoadedMesh); 

	//		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Mesh Load")));

	//		if (LoadedAnim)
	//		{
	//			GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	//			GetMesh()->SetAnimation(LoadedAnim);
	//			GetMesh()->Play(true);

	//			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Animation Load")));
	//		}
	//		else
	//		{
	//			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Animation Load Failed ")));
	//		}
	//	}
	//	else {
	//		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Mesh Load Failed ")));
	//	}
	//}

	//if (currentPreset == "InpermonPreset.uasset")
	//{
	//	FSoftObjectPath MeshPath(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Props/SM_TableRound.SM_TableRound'"));

	//	USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshPath.TryLoad());
	//	if (LoadedMesh)
	//	{
	//		GetMesh()->SetSkeletalMesh(LoadedMesh);
	//	}
	//}

	//if (currentPreset == "FreezardPreset.uasset")
	//{
	//	FSoftObjectPath MeshPath(TEXT("/Script/Engine.SkeletalMesh'/Game/Animation/Boar/Boar_idle_test3s_4.Boar_idle_test3s_4'"));
	//	FSoftObjectPath AnimPath(TEXT("/Script/Engine.AnimSequence'/Game/Animation/Boar/Boar_idle_test3s_2_Anim.Boar_idle_test3s_2_Anim'"));

	//	USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshPath.TryLoad());
	//	UAnimSequence* LoadedAnim = Cast<UAnimSequence>(AnimPath.TryLoad());

	//	if (LoadedMesh)
	//	{
	//		GetMesh()->SetSkeletalMesh(LoadedMesh);
	//		if (LoadedAnim)
	//		{
	//			GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	//			GetMesh()->SetAnimation(LoadedAnim);
	//			GetMesh()->Play(true);

	//			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Animation Load")));
	//		}
	//		else
	//		{
	//			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Animation Load Failed ")));
	//		}
	//	}
	//}

	//if (currentPreset == "StoneGolemPreset.uasset")
	//{
	//	FSoftObjectPath MeshPath(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Props/SM_Lamp_Ceiling.SM_Lamp_Ceiling'"));

	//	USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshPath.TryLoad());
	//	if (LoadedMesh)
	//	{
	//		GetMesh()->SetSkeletalMesh(LoadedMesh);
	//	}
	//}
}
