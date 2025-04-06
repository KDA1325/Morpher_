#include "EntityPreset.h"
#include "EntityWidget.h"
#include "MyAIController.h"
#include "MyAI.h"
#include "BehaviorTree/BlackboardComponent.h"

AEntityPreset::AEntityPreset()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AMyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	CurrentHP = 0;
	currentSpeed = 0;
	MaxHp = 100.0f;

	NormalSkillHitBox = nullptr;
}

// Called when the game starts or when spawned
void AEntityPreset::BeginPlay()
{
	Super::BeginPlay();
	// WidgetComponent�� ���� ���� �ν��Ͻ��� ������ EntityWidget�� �Ҵ�
	FName WidgetCompName = TEXT("EntityPresetWidget");
	WidgetComp = Cast<UWidgetComponent>(GetDefaultSubobjectByName(WidgetCompName));  // ��� ���� WidgetComp ���

	if (WidgetComp)
	{
		UUserWidget* UserWidget = WidgetComp->GetUserWidgetObject();
		if (UserWidget)
		{
			EntityWidget = Cast<UEntityWidget>(UserWidget);
			if (EntityWidget)
			{
				OnHealthChanged.Broadcast(CurrentHP);

				EntityWidget->EntityPreset = this;
				// ü�� ���� ��������Ʈ ���ε�
				OnHealthChanged.AddDynamic(EntityWidget, &UEntityWidget::UpdateHealthBar);
				UE_LOG(LogTemp, Warning, TEXT("banana HP Bar connected %s"), *GetName());
			}
		/*	else
			{
				UE_LOG(LogTemp, Error, TEXT("bananaBeginPlay WidgetComp's widget is not of type UEntityWidget for %s"), *GetName());
			}*/
		}
		/*else
		{
			UE_LOG(LogTemp, Error, TEXT("bananaBeginPlay WidgetComp exists, but no widget instance was created for %s"), *GetName());
		}*/
	}
	//else
	//{
	//	UE_LOG(LogTemp, Error, TEXT("bananaBeginPlay WidgetComp is not assigned in blueprint for %s"), *GetName());
	//}

}

// Called every frame
void AEntityPreset::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEntityPreset::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float AEntityPreset::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{	
	UE_LOG(LogTemp, Log, TEXT("banana Damage: %f"), DamageAmount);
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser); // �θ� Ŭ������ TakeDamage ȣ��
	UE_LOG(LogTemp, Log, TEXT("banana (TakeDamage)CurrentHP: %f, DamageAmount: %f, CurrentHP - DamageAmount: %f"), CurrentHP, DamageAmount, CurrentHP - DamageAmount);


	if (CurrentHP > 0)
	{
		ApplyDamage(DamageAmount);
	}
	else
	{
		//UE_LOG(LogTemp, Log, TEXT("banana Entity Died!"));
				//Destroy();
	}
	return ActualDamage;
}

void AEntityPreset::SetHP(float NewHP)
{
	CurrentHP = FMath::Clamp(NewHP, 0.0f ,MaxHp);
	//CurrentHP = NewHP;
	// ��������Ʈ ȣ��
	//UE_LOG(LogTemp, Warning, TEXT("banana SetHP - NewHP: %f, ClampedHP: %f, MaxHP: %f"), NewHP, CurrentHP, MaxHp);
	OnHealthChanged.Broadcast(CurrentHP);

	if (CurrentHP <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("banana Entity Die"));
		Destroy();
	}
}


void AEntityPreset::ApplyDamage(float DamageAmount)
{
	SetHP(CurrentHP - DamageAmount);
}

float AEntityPreset::GetHPRatio()
{
	if (MaxHp > 0)
	{
		//UE_LOG(LogTemp, Log, TEXT("banana MaxHp > 0"));
		return(CurrentHP / MaxHp);
	}
	else {
		//UE_LOG(LogTemp, Log, TEXT("banana MaxHp = 0"));
		return 0.0f;
	}
}

void AEntityPreset::SetMoveSpeed(int32 MoveSpeed)
{
	currentSpeed = MoveSpeed;
}

void AEntityPreset::InitializeEntity(FABEntityData& InEntityData)
{	
	//EntityData = InEntityData;

	// Entity 정보 초기화 
	MaxHp = InEntityData.HP;
	SetActorLabel(InEntityData.EntityName);
	SetMoveSpeed(InEntityData.MoveSpeed);
	SetHP(InEntityData.HP);

	UE_LOG(LogTemp, Error, TEXT("banana Initialized Entity with Name: %s, HP: %d, Move Speed: %d"),
		*InEntityData.EntityName, InEntityData.HP, InEntityData.MoveSpeed);

	// EntityData에 저장된 Normal Skill 식별자를 통해 스킬 데이터 가져옴 
	FSkillData NormalSkillData;
	if (UABGameSingleton::Get().GetSkillDataBySkillID(InEntityData.NormalSkill, NormalSkillData))
	{
		UE_LOG(LogTemp, Warning, TEXT("Loaded Skill Data: %s, Skill Type: %d, Skill Range: %f"),
			*NormalSkillData.SkillNameID, (uint8)NormalSkillData.SkillType, NormalSkillData.SkillRange);

		// SkillType이 HitBox라면 
		if (NormalSkillData.SkillType == EnumSkillType::HitBox)
		{
			SetupHitBoxComponent(NormalSkillData);
		}
	}

	// NormalSkillData에 저장된 SkillNameID 식별자를 통해 스킬 효과 데이터 가져옴
	FSkillEffectData NormalSkillEffectData;
	if (UABGameSingleton::Get().GetSkillEffectDataTBySkillID(NormalSkillData.SkillNameID, NormalSkillEffectData))
	{
		UE_LOG(LogTemp, Warning, TEXT("Loaded Skill Effect Data: %s, Effect Type: %d, Effect Value: %f"),
			*NormalSkillEffectData.SkillNameID, (uint8)NormalSkillEffectData.EffectType, NormalSkillEffectData.EffectValue01);
	}
}

// Normal 스킬의 히트박스 컴포넌트를 생성 및 설정하는 함수 
void AEntityPreset::SetupHitBoxComponent(FSkillData& SkillData)
{
	if (SkillData.SkillTypeShape == EnumSkillTypeShape::Box)
	{
		// 히트박스 컴포넌트 생성 
		if (!NormalSkillHitBox)
		{
			NormalSkillHitBox = NewObject<UBoxComponent>(this, TEXT("NormalSkillHitBox"));

			if (NormalSkillHitBox)
			{
				// 컴포넌트 등록
				NormalSkillHitBox->RegisterComponent();

				// 스켈레탈 메시 소켓에 부착 
				NormalSkillHitBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("BiteHitBox"));
			}
		}

		// 히트박스 컴포넌트 설정 
		if (NormalSkillHitBox)
		{
			NormalSkillHitBox->SetHiddenInGame(false);
			NormalSkillHitBox->SetVisibility(true);
			UE_LOG(LogTemp, Warning, TEXT("HitBox is now visible"));

			FVector HalfExtent = FVector(SkillData.SkillTypeSizeX / 2, SkillData.SkillTypeSizeY / 2, 50.0f);
			NormalSkillHitBox->SetBoxExtent(HalfExtent);

			// 전방 길이가 SkillTypeSizeX 전체가 되도록 히트박스 중심을 X축 방향으로 옮김
			FVector NewRelativeLocation = FVector(SkillData.SkillTypeSizeX / 2, 0, 0);
			NormalSkillHitBox->SetRelativeLocation(NewRelativeLocation);
		}
	}

	//if (SkillData.SkillTypeShape == EnumSkillTypeShape::Sphere)
	//{
	//	// 히트박스 컴포넌트 생성 
	//	if (!NormalSkillHitBox)
	//	{
	//		NormalSkillHitBox = NewObject<UBoxComponent>(this, TEXT("NormalSkillHitBox"));

	//		if (NormalSkillHitBox)
	//		{
	//			// 컴포넌트 등록
	//			NormalSkillHitBox->RegisterComponent();

	//			// 스켈레탈 메시 소켓에 부착 
	//			NormalSkillHitBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("BiteHitBox"));
	//		}
	//	}

	//	// 히트박스 컴포넌트 설정 
	//	if (NormalSkillHitBox)
	//	{
	//		FVector HalfExtent = FVector(SkillData.SkillTypeSizeX / 2, SkillData.SkillTypeSizeY / 2, 50.0f);
	//		NormalSkillHitBox->SetBoxExtent(HalfExtent);

	//		// 전방 길이가 SkillTypeSizeX 전체가 되도록 히트박스 중심을 X축 방향으로 옮김
	//		FVector NewRelativeLocation = FVector(SkillData.SkillTypeSizeX / 2, 0, 0);
	//		NormalSkillHitBox->SetRelativeLocation(NewRelativeLocation);
	//	}
	//}	
}
//
//float AEntityPreset::GetAIPatrolRadius()
//{
//	return 800.0f; // 8����
//}
//
//float AEntityPreset::GetAIDetectRange()
//{
//	return 900.0f; // 8����
//}
//
//float AEntityPreset::GetAIAttackRange()
//{
//	return 0.0f;
//}
//
//float AEntityPreset::GetAITurnSpeed()
//{
//	return 0.0f;
//}

EnumAttackType AEntityPreset::GetAttackType()
{
	UE_LOG(LogTemp, Warning, TEXT("Get AttackType: %d"), currentAttackType);

	return currentAttackType;
}
