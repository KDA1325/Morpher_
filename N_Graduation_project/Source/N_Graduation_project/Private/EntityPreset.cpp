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

void AEntityPreset::SetupHitBoxComponent(FSkillData& SkillData)
{
	if (SkillData.SkillTypeShape == EnumSkillTypeShape::Box)
	{
		// 1. HitBoxContainer 생성 및 소켓에 부착
		if (!HitBoxContainer)
		{
			HitBoxContainer = NewObject<USceneComponent>(this, TEXT("HitBoxContainer"));
			if (HitBoxContainer)
			{
				HitBoxContainer->RegisterComponent();
				// 스켈레탈 메시의 소켓에 부착
				HitBoxContainer->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("BiteHitBox"));
				// 컨테이너는 소켓의 원점을 그대로 유지 (즉, 히트박스가 소켓 위치에서 시작)
			}
		}

		// 2. NormalSkillHitBox 생성 (HitBoxContainer의 자식)
		if (!NormalSkillHitBox)
		{
			NormalSkillHitBox = NewObject<UBoxComponent>(this, TEXT("NormalSkillHitBox"));
			if (NormalSkillHitBox)
			{
				NormalSkillHitBox->RegisterComponent();
				NormalSkillHitBox->AttachToComponent(HitBoxContainer, FAttachmentTransformRules::KeepRelativeTransform);
			}
		}

		if (NormalSkillHitBox && HitBoxContainer)
		{
			// HitBox를 보이도록 설정
			NormalSkillHitBox->SetHiddenInGame(false);
			NormalSkillHitBox->SetVisibility(true);

			// UBoxComponent는 half extents를 사용
			// 전방 길이로 SkillTypeSizeX를 전체 길이로 보고, 이 값을 절반으로 해서 half extent로 사용
			// half extent의 X축 값은 히트박스의 Y 크기 값(SkillTypeSizeY/2),
			// Y축은 Z 크기 값,
			// Z축은 X 크기 값(SkillTypeSizeX/2)
			FVector HalfExtent = FVector(SkillData.SkillTypeSizeY / 2.0f, 50.0f, SkillData.SkillTypeSizeX / 2.0f);
			NormalSkillHitBox->SetBoxExtent(HalfExtent);

			// 기본 UBoxComponent는 자신의 중심을 기준으로 확장
			// 기획서에 따라 소켓(HitBoxContainer)의 원점에서부터 전방(X축)으로만 확장되도록 하기
			// UBoxComponent를 HitBoxContainer의 자식으로 두고, 상대 위치를 Z축(+X 방향)으로 half extent만큼 이동
			FVector NewRelativeLocation = FVector(0.0f, 0.0f, HalfExtent.X);
			NormalSkillHitBox->SetRelativeLocation(NewRelativeLocation);

			UE_LOG(LogTemp, Warning, TEXT("SetupHitBoxComponent: HalfExtent=%s, NewRelativeLocation=%s"),
				*HalfExtent.ToString(), *NewRelativeLocation.ToString());
		}
	}

	if (SkillData.SkillTypeShape == EnumSkillTypeShape::Sphere)
	{
		
	}
}

EnumAttackType AEntityPreset::GetAttackType()
{
	UE_LOG(LogTemp, Warning, TEXT("Get AttackType: %d"), currentAttackType);

	return currentAttackType;
}
