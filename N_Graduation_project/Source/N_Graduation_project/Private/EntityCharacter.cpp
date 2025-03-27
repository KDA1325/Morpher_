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

	// 몽타주 포인터 초기화
	NormalSkillMontage = nullptr;
	SpecialSkillMontage = nullptr;
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

	// Entity 데이터에 따라 초기화 
	SetActorLabel(InEntityData.EntityName);
	SetMaxHp(InEntityData.HP);
	SetMoveSpeed(InEntityData.MoveSpeed);

	// 메시 및 애니메이션 설정 
	SetPreset(InEntityData.PresetReference);

	UE_LOG(LogTemp, Warning, TEXT("Initialized Entity with Name: %s, HP: %d, Move Speed: %d"),
		*InEntityData.EntityName, InEntityData.HP, InEntityData.MoveSpeed);
	
	SetWidget();
}

void AEntityCharacter::SetPreset(FString PresetReference)
{
	currentPreset = PresetReference;

	// 프리셋 이름과 경로를 매핑하는 맵 선언

	// SkeletalMesh 경로 
	TMap<FString, FSoftObjectPath> SkeletalMeshPaths = {
		{"PCPreset.uasset", FSoftObjectPath(TEXT("/Game/Animation/Boar/Boar_idle_test3s_3.Boar_idle_test3s_3"))},
		{"WildBoarPreset.uasset", FSoftObjectPath(TEXT("/Game/Animation/Boar/Boar_idle_test3s_2.Boar_idle_test3s_2"))},
		{"InpermonPreset.uasset", FSoftObjectPath(TEXT("/Game/Animation/Boar/Boar_idle_test3s_2.Boar_idle_test3s_2"))},
		{"FreezardPreset.uasset", FSoftObjectPath(TEXT("/Game/Animation/Boar/Boar_idle_test3s_4.Boar_idle_test3s_4"))},
		{"StoneGolemPreset.uasset", FSoftObjectPath(TEXT("/Game/SyntyAsset/PolygonFantasyRivals/Meshes/New_Characters/SK_BR_Character_ElementalGolem_01.SK_BR_Character_ElementalGolem_01"))},
		{"SkeletonWarriorPreset.uasset", FSoftObjectPath(TEXT("/Game/SyntyAsset/PolygonDarkFantasy/Meshes/Characters/SK_Chr_Skeleton_LightArmor_01.SK_Chr_Skeleton_LightArmor_01"))},
		{"SkeletonArcherPreset.uasset", FSoftObjectPath(TEXT("/Game/SyntyAsset/PolygonDarkFantasy/Meshes/Characters/SK_Chr_Skeleton_Ranger_01.SK_Chr_Skeleton_Ranger_01"))}
	};

	// AnimBP 경로 (Idle, Move 기본 루프 애니메이션은 AnimBP 적용)
	TMap<FString, FSoftObjectPath> AnimBPPaths = {
		{"PCPreset.uasset", FSoftObjectPath(TEXT("/Game/Animation/WildBoar_AnimBP.WildBoar_AnimBP"))},
		{"WildBoarPreset.uasset", FSoftObjectPath(TEXT("/Game/Animation/WildBoar_AnimBP.WildBoar_AnimBP"))},
		{"FreezardPreset.uasset", FSoftObjectPath(TEXT("/Game/Animation/WildBoar_AnimBP.WildBoar_AnimBP"))},
		{"InpermonPreset.uasset", FSoftObjectPath(TEXT("/Game/Animation/WildBoar_AnimBP.WildBoar_AnimBP"))},
		{"StoneGolemPreset.uasset", FSoftObjectPath(TEXT("/Game/SyntyAsset/PolygonFantasyRivals/EpicContent/Mannequin/Animations/ThirdPerson_AnimBP.ThirdPerson_AnimBP"))},
		{"SkeletonWarriorPreset.uasset", FSoftObjectPath(TEXT("/Game/SyntyAsset/PolygonFantasyRivals/EpicContent/Mannequin/Animations/ThirdPerson_AnimBP.ThirdPerson_AnimBP"))},
		{"SkeletonArcherPreset.uasset", FSoftObjectPath(TEXT("/Game/SyntyAsset/PolygonFantasyRivals/EpicContent/Mannequin/Animations/ThirdPerson_AnimBP.ThirdPerson_AnimBP"))}
	};
	
	// NormalSkillMontage 경로 (일반 스킬 애니메이션)
	TMap<FString, FSoftObjectPath> NormalSkillMontagePaths = {
		{"PCPreset.uasset", FSoftObjectPath(TEXT("/Game/Animation/WildBoar_Montage.WildBoar_Montage"))},
		{"WildBoarPreset.uasset", FSoftObjectPath(TEXT("/Game/Animation/WildBoar_Montage.WildBoar_Montage"))},
		{"FreezardPreset.uasset", FSoftObjectPath(TEXT("/Game/Animation/WildBoar_Montage.WildBoar_Montage"))},
		{"InpermonPreset.uasset", FSoftObjectPath(TEXT("/Game/Animation/WildBoar_Montage.WildBoar_Montage"))},
		{"StoneGolemPreset.uasset", FSoftObjectPath(TEXT("/Game/Animation/StoneGolem_Montage.StoneGolem_Montage"))},
		{"SkeletonWarriorPreset.uasset", FSoftObjectPath(TEXT("/Game/Animation/SkeletonWarrior_Montage.SkeletonWarrior_Montage"))},
		{"SkeletonArcherPreset.uasset", FSoftObjectPath(TEXT("/Game/Animation/SkeletonArcher_Montage.SkeletonArcher_Montage"))}
	};
	
	// SpecialSkillMontage 경로 (특수 스킬 애니메이션)
	TMap<FString, FSoftObjectPath> SpecialSkillMontagePaths = {
		{"PCPreset.uasset", FSoftObjectPath(TEXT("/Game/Animation/WildBoar_Montage.WildBoar_Montage"))},
		{"WildBoarPreset.uasset", FSoftObjectPath(TEXT("/Game/Animation/WildBoar_Montage.WildBoar_Montage"))},
		{"FreezardPreset.uasset", FSoftObjectPath(TEXT("/Game/Animation/WildBoar_Montage.WildBoar_Montage"))},
		{"InpermonPreset.uasset", FSoftObjectPath(TEXT("/Game/Animation/WildBoar_Montage.WildBoar_Montage"))},
		{"StoneGolemPreset.uasset", FSoftObjectPath(TEXT("/Game/Animation/StoneGolem_Montage.StoneGolem_Montage"))},
		{"SkeletonWarriorPreset.uasset", FSoftObjectPath(TEXT("/Game/Animation/SkeletonWarrior_Montage.SkeletonWarrior_Montage"))},
		{"SkeletonArcherPreset.uasset", FSoftObjectPath(TEXT("/Game/Animation/SkeletonArcher_Montage.SkeletonArcher_Montage"))}
	};

	// Skeletal Mesh 설정
	if (SkeletalMeshPaths.Contains(currentPreset))
	{
		FSoftObjectPath MeshPath = SkeletalMeshPaths[currentPreset];
		USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshPath.TryLoad());
		if (LoadedMesh)
		{
			GetMesh()->SetSkeletalMesh(LoadedMesh);
			
			// 예: 메시가 +Y를 전방으로 보고 있다면 -90도 돌려 +X를 앞방향으로 맞춤
			GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("Skeletal Mesh Loaded Successfully"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Skeletal Mesh Load Failed: %s"), *MeshPath.ToString());
		}
	}
	
	// AnimBP 설정
	if (AnimBPPaths.Contains(currentPreset))
	{
		FSoftObjectPath AnimBPPath = AnimBPPaths[currentPreset];
		UObject* LoadedObj = AnimBPPath.TryLoad();
		if (LoadedObj)
		{
			UAnimBlueprint* AnimBP = Cast<UAnimBlueprint>(LoadedObj);
			UClass* AnimInstanceClass = nullptr;
			if (AnimBP)
			{
				AnimInstanceClass = AnimBP->GeneratedClass;
			}
			else
			{
				AnimInstanceClass = Cast<UClass>(LoadedObj);
			}
			if (AnimInstanceClass)
			{
				GetMesh()->SetAnimInstanceClass(AnimInstanceClass);
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("Animation Loaded Successfully"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Animation Load Failed: %s"), *AnimBPPath.ToString());
			}
		}
	}
	
	// NormalSkillMontage 설정 
	if (NormalSkillMontagePaths.Contains(currentPreset))
	{
		FSoftObjectPath MontagePath = NormalSkillMontagePaths[currentPreset];
		NormalSkillMontage = Cast<UAnimMontage>(MontagePath.TryLoad());
		if (!NormalSkillMontage)
		{
			UE_LOG(LogTemp, Error, TEXT("Animation Load Failed: %s"), *MontagePath.ToString());
		}
	}
	
	// SpecialSkillMontage 설정 
	if (SpecialSkillMontagePaths.Contains(currentPreset))
	{
		FSoftObjectPath MontagePath = SpecialSkillMontagePaths[currentPreset];
		SpecialSkillMontage = Cast<UAnimMontage>(MontagePath.TryLoad());
		if (!SpecialSkillMontage)
		{
			UE_LOG(LogTemp, Error, TEXT("Animation Load Failed: %s"), *MontagePath.ToString());
		}
	}

	//if (SkeletalMeshPaths.Contains(currentPreset))
	//{
	//	FSoftObjectPath MeshPath = SkeletalMeshPaths[currentPreset];
	//	USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshPath.TryLoad());
	//	if (LoadedMesh)
	//	{
	//		GetMesh()->SetSkeletalMesh(LoadedMesh);
	//		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("Skeletal Mesh Loaded Successfully"));

	//		// 애니메이션 처리
	//		if (AnimBPPaths.Contains(currentPreset))
	//		{
	//			FSoftObjectPath AnimPath = AnimBPPaths[currentPreset];
	//			UAnimSequence* LoadedAnim = Cast<UAnimSequence>(AnimPath.TryLoad());
	//			if (LoadedAnim)
	//			{
	//				GetMesh()->SetAnimationMode(EAnimationMode::AnimationCustomMode);
	//				//GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	//				GetMesh()->SetAnimation(LoadedAnim);
	//				GetMesh()->Play(true);
	//				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("Animation Loaded Successfully"));
	//			}
	//			else
	//			{
	//				UE_LOG(LogTemp, Error, TEXT("Animation Load Failed: %s"), *AnimPath.ToString());
	//			}
	//		}
	//	}
	//	else
	//	{
	//		UE_LOG(LogTemp, Error, TEXT("Skeletal Mesh Load Failed: %s"), *MeshPath.ToString());
	//	}
	//}

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

void AEntityCharacter::SetWidget()
{
	// 위젯 컴포넌트 불러오기 
	UWidgetComponent* WidgetComponent = FindComponentByClass<UWidgetComponent>();

	if (WidgetComponent)
	{
		UUserWidget* UserWidget = WidgetComponent->GetWidget();

		if (UEntityWidget* MyEntityWidget = Cast<UEntityWidget>(UserWidget))
		{
			MyEntityWidget->UpdateHealthBar(currentHp);
			//MyEntityWidget->ReceiveEntityName(FText::FromString(EntityData.EntityName));
			//MyEntityWidget->ReceiveEntitySpeed(currentSpeed);
			UE_LOG(LogTemp, Warning, TEXT("Widget updated successfully"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to cast UserWidget to UEntityWidget"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("WidgetComponent not found"));
	}
}

float AEntityCharacter::GetAIPatrolRadius()
{
	return 800.0f; // 8미터
}

float AEntityCharacter::GetAIDetectRange()
{
	return 900.0f; // 8미터
}

float AEntityCharacter::GetAIAttackRange()
{
	return 0.0f;
}

float AEntityCharacter::GetAITurnSpeed()
{
	return 0.0f;
}
