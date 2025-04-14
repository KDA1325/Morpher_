#include "EntityPreset.h"
#include "EntityWidget.h"
#include "EntitySkillComponent.h"
#include "MyAIController.h"
#include "MyAI.h"
#include "Navigation/PathFollowingComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AEntityPreset::AEntityPreset()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	EntitySkillComponent = CreateDefaultSubobject<UEntitySkillComponent>(TEXT("EntitySkillComponent"));

	AIControllerClass = AMyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	CurrentHP = 0;
	currentSpeed = 0;
	MaxHp = 100.0f;

	NormalSkillHitBox = nullptr;
	SpecialSkillHitBox = nullptr;

	NormalHitBoxContainer = nullptr;
	SpecialHitBoxContainer = nullptr;

	GetCharacterMovement()->BrakingFrictionFactor = 0.0f; // 멈출 때 마찰 없음
	GetCharacterMovement()->GroundFriction = 0.0f;
}

// Called when the game starts or when spawned
void AEntityPreset::BeginPlay()
{
	Super::BeginPlay();
	// WidgetComponent를 통해 위젯 인스턴스를 가져와 EntityWidget에 할당
	FName WidgetCompName = TEXT("EntityPresetWidget");
	WidgetComp = Cast<UWidgetComponent>(GetDefaultSubobjectByName(WidgetCompName));  // 멤버 변수 WidgetComp 사용

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
				// 체력 변경 델리게이트 바인딩
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
	
	// 1. HitBoxContainer 생성 및 소켓에 부착
	if (!NormalHitBoxContainer)
	{
		NormalHitBoxContainer = NewObject<USceneComponent>(this, TEXT("NormalHitBoxContainer"));
		if (NormalHitBoxContainer)
		{
			// 컨테이너를 컴포넌트로 등록 
			NormalHitBoxContainer->RegisterComponent();

			// 스켈레탈 메시의 소켓에 부착
			// 컨테이너는 소켓의 원점을 그대로 유지 (즉, 히트박스가 소켓 위치에서 시작)
			NormalHitBoxContainer->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("BiteHitBox"));
		}
	}
	if (!SpecialHitBoxContainer)
	{
		SpecialHitBoxContainer = NewObject<USceneComponent>(this, TEXT("SpecialHitBoxContainer"));
		if (SpecialHitBoxContainer)
		{
			// 컨테이너를 컴포넌트로 등록 
			SpecialHitBoxContainer->RegisterComponent();

			// 스켈레탈 메시의 소켓에 부착
			// 컨테이너는 소켓의 원점을 그대로 유지 (즉, 히트박스가 소켓 위치에서 시작)
			SpecialHitBoxContainer->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("ChargeHitBox"));
		}
	}

}

// Called every frame
void AEntityPreset::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsCharging)
	{
		float TravelledDistance = FVector::Dist(ChargeStartLocation, GetActorLocation());
		if (TravelledDistance >= ChargeDistance)
		{
			StopMovement();
			bIsCharging = false;

			HideSpecialHitBox(); // 돌진 끝났으면 히트박스 제거
			UE_LOG(LogTemp, Warning, TEXT("Charge Finished — Distance Reached"));
		}
	}
}

// Called to bind functionality to input
void AEntityPreset::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float AEntityPreset::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{	
	UE_LOG(LogTemp, Log, TEXT("banana Damage: %f"), DamageAmount);
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser); // 부모 클래스의 TakeDamage 호출
	UE_LOG(LogTemp, Log, TEXT("banana (TakeDamage)CurrentHP: %f, DamageAmount: %f, CurrentHP - DamageAmount: %f"), CurrentHP, DamageAmount, CurrentHP - DamageAmount);


	if (CurrentHP > 0)
	{
		ApplyDamage(DamageAmount);
	}

	return ActualDamage;
}

void AEntityPreset::SetHP(float NewHP)
{
	CurrentHP = FMath::Clamp(NewHP, 0.0f ,MaxHp);
	//CurrentHP = NewHP;
	// 델리게이트 호출
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

void AEntityPreset::SetNormalSkillRange(float NormalSkillRange)
{
	currentNormalSkillRange = NormalSkillRange;
}

void AEntityPreset::SetSpecialSkillRange(float SpecialSkillRange)
{
	currentSpecialSkillRange = SpecialSkillRange;
}

void AEntityPreset::InitializeEntity(FABEntityData& InEntityData)
{	
	// Entity 데이터에 따라 초기화 
	MaxHp = InEntityData.HP;
	SetActorLabel(InEntityData.EntityName);
	SetMoveSpeed(InEntityData.MoveSpeed);
	SetHP(InEntityData.HP);

	UE_LOG(LogTemp, Error, TEXT("banana Initialized Entity with Name: %s, HP: %d, Move Speed: %d"),
		*InEntityData.EntityName, InEntityData.HP, InEntityData.MoveSpeed);

	// EntityData에 저장된 Normal Skill 식별자를 통해 스킬 데이터 가져옴 
	if (UABGameSingleton::Get().GetSkillDataBySkillID(InEntityData.NormalSkill, NormalSkillData))
	{
		UE_LOG(LogTemp, Warning, TEXT("Loaded Skill Data: %s, Skill Type: %d, Skill Range: %f"),
			*NormalSkillData.SkillNameID, (uint8)NormalSkillData.SkillType, NormalSkillData.SkillRange);

		SetNormalSkillRange(NormalSkillData.SkillRange);

		// SkillType이 HitBox라면 
		if (NormalSkillData.SkillType == EnumSkillType::HitBox)
		{
			SetupHitBoxComponent(NormalSkillData);
		}

		// NormalSkillData에 저장된 SkillNameID 식별자를 통해 스킬 효과 데이터 가져옴
		if (UABGameSingleton::Get().GetSkillEffectDataBySkillID(NormalSkillData.SkillNameID, NormalSkillEffectData))
		{
			for (const FSkillEffectData& EffectData : NormalSkillEffectData)
			{
				UE_LOG(LogTemp, Warning, TEXT("Loaded Skill Effect Data: %s, Effect Type: %d, Effect Value01: %f, Effect Value02: %f"),
					*EffectData.SkillNameID, (uint8)EffectData.EffectType, EffectData.EffectValue01, EffectData.EffectValue02);
			}			
		}
	}
	
	// EntityData에 저장된 Special Skill 식별자를 통해 스킬 데이터 가져옴 
	if (UABGameSingleton::Get().GetSkillDataBySkillID(InEntityData.SpecialSkill, SpecialSkillData))
	{
		UE_LOG(LogTemp, Warning, TEXT("Loaded Skill Data: %s, Skill Type: %d, Skill Range: %f"),
			*SpecialSkillData.SkillNameID, (uint8)SpecialSkillData.SkillType, SpecialSkillData.SkillRange);

		SetSpecialSkillRange(SpecialSkillData.SkillRange);

		// SkillType이 HitBox라면 
		if (SpecialSkillData.SkillType == EnumSkillType::HitBox)
		{
			SetupHitBoxComponent(SpecialSkillData);
		}

		// SpecialSkillData에 저장된 SkillNameID 식별자를 통해 스킬 효과 데이터 가져옴
		if (UABGameSingleton::Get().GetSkillEffectDataBySkillID(SpecialSkillData.SkillNameID, SpecialSkillEffectData))
		{
			for (const FSkillEffectData& EffectData : SpecialSkillEffectData)
			{

				UE_LOG(LogTemp, Warning, TEXT("Loaded Skill Effect Data: %s, Effect Type: %d, Effect Value01: %f, Effect Value02: %f"),
					*EffectData.SkillNameID, (uint8)EffectData.EffectType, EffectData.EffectValue01, EffectData.EffectValue02);
			}
		}
	}
}

void AEntityPreset::SetupHitBoxComponent(FSkillData& SkillData)
{
	if (SkillData.SkillTypeShape == EnumSkillTypeShape::Box)
	{
		// SkillNameID -> 소켓 이름 매핑 테이블
		TMap<FString, FName> SkillToSocketMap = {
			//{ "Skill_Slash", TEXT("SlashSocket") },
			{ "Skill_Bite", TEXT("BiteHitBox") },
			{ "Skill_Charge", TEXT("ChargeHitBox") }
			//{ "Skill_TailSwing", TEXT("TailSocket") },
			//{ "Skill_FreezeBreath", TEXT("MouthSocket") },
			//{ "Skill_ArmSwing", TEXT("RightArmSocket") },
			//{ "Skill_EarthBreaker", TEXT("FootSocket") }
		};

		FName SocketToAttach = TEXT("DefaultHitBox"); 
		if (FName* FoundSocket = SkillToSocketMap.Find(SkillData.SkillNameID))
		{
			SocketToAttach = *FoundSocket;
		}

		USceneComponent* Container = nullptr;
		if (SkillData.SkillNameID == "Skill_Charge")
		{
			Container = SpecialHitBoxContainer;
		}
		else
		{
			Container = NormalHitBoxContainer;
		}

		Container->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, SocketToAttach);

		// 스킬 타입에 따라 해당 히트박스 컴포넌트 생성
		if (SkillData.SkillNameID == "Skill_Charge")
		{
			// Special 히트박스 생성
			if (!SpecialSkillHitBox)
			{
				SpecialSkillHitBox = NewObject<UBoxComponent>(this, TEXT("SpecialSkillHitBox"));
				if (SpecialSkillHitBox)
				{
					SpecialSkillHitBox->RegisterComponent();
					SpecialSkillHitBox->AttachToComponent(Container, FAttachmentTransformRules::KeepRelativeTransform);
					HideSpecialHitBox();

					ConfigureHitBox(SpecialSkillHitBox);
					SpecialSkillHitBox->OnComponentBeginOverlap.AddDynamic(this, &AEntityPreset::OnSpecialHitBoxOverlap);
				}
			}

			// UBoxComponent는 half extents를 사용
				// 전방 길이로 SkillTypeSizeX를 전체 길이로 보고, 이 값을 절반으로 해서 half extent로 사용
				// half extent의 X축 값은 히트박스의 Y 크기 값(SkillTypeSizeY/2),
				// Y축은 Z 크기 값,
				// Z축은 X 크기 값(SkillTypeSizeX/2)
			FVector HalfExtent = FVector(SkillData.SkillTypeSizeY / 2.0f, 50.0f, SkillData.SkillTypeSizeX / 2.0f);
			SpecialSkillHitBox->SetBoxExtent(HalfExtent);

			// 기본 UBoxComponent는 자신의 중심을 기준으로 확장
			// 기획서에 따라 소켓(HitBoxContainer)의 원점에서부터 전방(X축)으로만 확장되도록 하기
			// UBoxComponent를 HitBoxContainer의 자식으로 두고, 상대 위치를 Z축(+X 방향)으로 half extent만큼 이동
			FVector NewRelativeLocation = FVector(0.0f, 0.0f, HalfExtent.X);
			SpecialSkillHitBox->SetRelativeLocation(NewRelativeLocation);

			UE_LOG(LogTemp, Warning, TEXT("SetupSpecialHitBoxComponent: HalfExtent=%s, NewRelativeLocation=%s"),
				*HalfExtent.ToString(), *NewRelativeLocation.ToString());
		}
		else
		{
			// Normal 히트박스 생성
			if (!NormalSkillHitBox)
			{
				NormalSkillHitBox = NewObject<UBoxComponent>(this, TEXT("NormalSkillHitBox"));
				if (NormalSkillHitBox)
				{
					NormalSkillHitBox->RegisterComponent();
					NormalSkillHitBox->AttachToComponent(NormalHitBoxContainer, FAttachmentTransformRules::KeepRelativeTransform);

					HideNormalHitBox();

					ConfigureHitBox(NormalSkillHitBox);

					// Overlap 이벤트 바인딩 
					NormalSkillHitBox->OnComponentBeginOverlap.AddDynamic(this, &AEntityPreset::OnNormalHitBoxOverlap);
				}
			}

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

			UE_LOG(LogTemp, Warning, TEXT("SetupNormalHitBoxComponent: HalfExtent=%s, NewRelativeLocation=%s"),
				*HalfExtent.ToString(), *NewRelativeLocation.ToString());
		}

		//// 1. HitBoxContainer 생성 및 소켓에 부착
		//if (!NormalHitBoxContainer)
		//{
		//	NormalHitBoxContainer = NewObject<USceneComponent>(this, TEXT("NormalHitBoxContainer"));
		//	if (NormalHitBoxContainer)
		//	{
		//		// 컨테이너를 컴포넌트로 등록 
		//		NormalHitBoxContainer->RegisterComponent();

		//		// 스켈레탈 메시의 소켓에 부착
		//		// 컨테이너는 소켓의 원점을 그대로 유지 (즉, 히트박스가 소켓 위치에서 시작)
		//		NormalHitBoxContainer->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, SocketToAttach);
		//	}
		//}
		//if (!SpecialHitBoxContainer)
		//{
		//	SpecialHitBoxContainer = NewObject<USceneComponent>(this, TEXT("SpecialHitBoxContainer"));
		//	if (SpecialHitBoxContainer)
		//	{
		//		// 컨테이너를 컴포넌트로 등록 
		//		SpecialHitBoxContainer->RegisterComponent();

		//		// 스켈레탈 메시의 소켓에 부착
		//		// 컨테이너는 소켓의 원점을 그대로 유지 (즉, 히트박스가 소켓 위치에서 시작)
		//		SpecialHitBoxContainer->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, SocketToAttach);
		//	}
		//}

		//// 2. NormalSkillHitBox 생성 (HitBoxContainer의 자식)
		//if (!NormalSkillHitBox)
		//{
		//	NormalSkillHitBox = NewObject<UBoxComponent>(this, TEXT("NormalSkillHitBox"));
		//	if (NormalSkillHitBox)
		//	{
		//		NormalSkillHitBox->RegisterComponent();
		//		NormalSkillHitBox->AttachToComponent(NormalHitBoxContainer, FAttachmentTransformRules::KeepRelativeTransform);

		//		HideNormalHitBox();

		//		// Overlap 이벤트 바인딩 
		//		NormalSkillHitBox->OnComponentBeginOverlap.AddDynamic(this, &AEntityPreset::OnNormalHitBoxOverlap);
		//	}
		//}

		//// 3. SpecialSkillHitBox 생성 (HitBoxContainer의 자식)
		//if (!SpecialSkillHitBox)
		//{
		//	SpecialSkillHitBox = NewObject<UBoxComponent>(this, TEXT("SpecialSkillHitBox"));
		//	if (SpecialSkillHitBox)
		//	{
		//		SpecialSkillHitBox->RegisterComponent();
		//		SpecialSkillHitBox->AttachToComponent(SpecialHitBoxContainer, FAttachmentTransformRules::KeepRelativeTransform);

		//		HideSpecialHitBox();

		//		// Overlap 이벤트 바인딩 
		//		SpecialSkillHitBox->OnComponentBeginOverlap.AddDynamic(this, &AEntityPreset::OnSpecialHitBoxOverlap);
		//	}
		//}

		//if (NormalSkillHitBox && NormalHitBoxContainer)
		//{
		//	// UBoxComponent는 half extents를 사용
		//	// 전방 길이로 SkillTypeSizeX를 전체 길이로 보고, 이 값을 절반으로 해서 half extent로 사용
		//	// half extent의 X축 값은 히트박스의 Y 크기 값(SkillTypeSizeY/2),
		//	// Y축은 Z 크기 값,
		//	// Z축은 X 크기 값(SkillTypeSizeX/2)
		//	FVector HalfExtent = FVector(SkillData.SkillTypeSizeY / 2.0f, 50.0f, SkillData.SkillTypeSizeX / 2.0f);
		//	NormalSkillHitBox->SetBoxExtent(HalfExtent);

		//	// 기본 UBoxComponent는 자신의 중심을 기준으로 확장
		//	// 기획서에 따라 소켓(HitBoxContainer)의 원점에서부터 전방(X축)으로만 확장되도록 하기
		//	// UBoxComponent를 HitBoxContainer의 자식으로 두고, 상대 위치를 Z축(+X 방향)으로 half extent만큼 이동
		//	FVector NewRelativeLocation = FVector(0.0f, 0.0f, HalfExtent.X);
		//	NormalSkillHitBox->SetRelativeLocation(NewRelativeLocation);

		//	UE_LOG(LogTemp, Warning, TEXT("SetupNormalHitBoxComponent: HalfExtent=%s, NewRelativeLocation=%s"),
		//		*HalfExtent.ToString(), *NewRelativeLocation.ToString());
		//}
		
		//if (SpecialSkillHitBox && SpecialHitBoxContainer)
		//{
		//	// UBoxComponent는 half extents를 사용
		//	// 전방 길이로 SkillTypeSizeX를 전체 길이로 보고, 이 값을 절반으로 해서 half extent로 사용
		//	// half extent의 X축 값은 히트박스의 Y 크기 값(SkillTypeSizeY/2),
		//	// Y축은 Z 크기 값,
		//	// Z축은 X 크기 값(SkillTypeSizeX/2)
		//	FVector HalfExtent = FVector(SkillData.SkillTypeSizeY / 2.0f, 50.0f, SkillData.SkillTypeSizeX / 2.0f);
		//	SpecialSkillHitBox->SetBoxExtent(HalfExtent);

		//	// 기본 UBoxComponent는 자신의 중심을 기준으로 확장
		//	// 기획서에 따라 소켓(HitBoxContainer)의 원점에서부터 전방(X축)으로만 확장되도록 하기
		//	// UBoxComponent를 HitBoxContainer의 자식으로 두고, 상대 위치를 Z축(+X 방향)으로 half extent만큼 이동
		//	FVector NewRelativeLocation = FVector(0.0f, 0.0f, HalfExtent.X);
		//	SpecialSkillHitBox->SetRelativeLocation(NewRelativeLocation);

		//	UE_LOG(LogTemp, Warning, TEXT("SetupSpecialHitBoxComponent: HalfExtent=%s, NewRelativeLocation=%s"),
		//		*HalfExtent.ToString(), *NewRelativeLocation.ToString());
		//}
	}

	if (SkillData.SkillTypeShape == EnumSkillTypeShape::Sphere)
	{
		UE_LOG(LogTemp, Warning, TEXT("Sphere"));
	}
}

// 히트박스 충돌 세팅 
void AEntityPreset::ConfigureHitBox(UBoxComponent* HitBox)
{
	if (!HitBox)
	{
		return;
	}

	HitBox->SetCollisionObjectType(ECC_WorldDynamic);
	HitBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	HitBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	HitBox->SetGenerateOverlapEvents(true);
}

void AEntityPreset::ShowNormalHitBox()
{
	float Duration = NormalSkillData.SkillDuration;

	// HitBox 활성화 
	NormalSkillHitBox->SetHiddenInGame(false);
	NormalSkillHitBox->SetVisibility(true);
	NormalSkillHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);  // 충돌 켜기, 물리는 무시하지만 쿼리만 처리
	UE_LOG(LogTemp, Warning, TEXT("Show HitBox"));

	// 유지 시간 이후 HideHitBox 함수 호출  
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AEntityPreset::HideNormalHitBox, Duration, false);
}

void AEntityPreset::HideNormalHitBox()
{
	// HitBox 비활성화 
	NormalSkillHitBox->SetHiddenInGame(true);
	NormalSkillHitBox->SetVisibility(false);
	NormalSkillHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);  // 충돌 끄기
	UE_LOG(LogTemp, Warning, TEXT("Hide HitBox"));
}

void AEntityPreset::ShowSpecialHitBox()
{
	float Duration = SpecialSkillData.SkillDuration;

	// HitBox 활성화 
	SpecialSkillHitBox->SetHiddenInGame(false);
	SpecialSkillHitBox->SetVisibility(true);
	SpecialSkillHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);  // 충돌 켜기
	UE_LOG(LogTemp, Warning, TEXT("Show HitBox"));

	// 유지 시간 이후 HideHitBox 함수 호출  
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AEntityPreset::HideSpecialHitBox, Duration, false);
}

void AEntityPreset::HideSpecialHitBox()
{
	// HitBox 비활성화 
	SpecialSkillHitBox->SetHiddenInGame(true);
	SpecialSkillHitBox->SetVisibility(false);
	SpecialSkillHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);  // 충돌 끄기
	UE_LOG(LogTemp, Warning, TEXT("Hide HitBox"));
}

void AEntityPreset::OnNormalHitBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		// 플레이어 캐릭터인지 확인
		ACharacter* PlayerCharacter = Cast<ACharacter>(OtherActor);
		if (PlayerCharacter)
		{
			// Normal 스킬에 부여된 모든 효과들을 반복 처리합니다.
			for (const FSkillEffectData& Effect : NormalSkillEffectData)
			{
				switch (Effect.EffectType)
				{
				case EnumEffectType::Damage:
				{
					float DamageToApply = Effect.EffectValue01;
					UGameplayStatics::ApplyDamage(OtherActor, DamageToApply, GetController(), this, nullptr);
					UE_LOG(LogTemp, Warning, TEXT("Normal HitBox Overlap: Applied Damage %f to %s"),
						DamageToApply, *OtherActor->GetName());
					break;
				}
				case EnumEffectType::KnockBack:
				{
					// KnockBack 효과 로직 구현 (예: OtherActor에게 일정 힘을 가해 뒤로 밀어냄)
					UE_LOG(LogTemp, Warning, TEXT("Normal HitBox Overlap: KnockBack effect applied to %s"),
						*OtherActor->GetName());
					break;
				}
				case EnumEffectType::Destroy:
				{
					// Destroy 효과가 적용되는 경우 OtherActor를 파괴하도록 합니다.
					//OtherActor->Destroy();
					UE_LOG(LogTemp, Warning, TEXT("Normal HitBox Overlap: Destroyed %s"),
						*OtherActor->GetName());
					break;
				}
				// 필요한 다른 효과에 대해서도 추가적으로 처리
				default:
					break;
				}
			}
		}
	}
}

//void AEntityPreset::OnSpecialHitBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	if (OtherActor && OtherActor != this)
//	{
//		// 플레이어 캐릭터인지 확인
//		ACharacter* PlayerCharacter = Cast<ACharacter>(OtherActor);
//		if (PlayerCharacter)
//		{
//			for (const FSkillEffectData& Effect : SpecialSkillEffectData)
//			{
//				switch (Effect.EffectType)
//				{
//				case EnumEffectType::Damage:
//				{
//					float DamageToApply = Effect.EffectValue01;
//					UGameplayStatics::ApplyDamage(OtherActor, DamageToApply, GetController(), this, nullptr);
//					UE_LOG(LogTemp, Warning, TEXT("Special HitBox Overlap: Applied Damage %f to %s"),
//						DamageToApply, *OtherActor->GetName());
//					break;
//				}
//				case EnumEffectType::KnockBack:
//				{
//					// KnockBack 효과 로직 구현
//					UE_LOG(LogTemp, Warning, TEXT("Special HitBox Overlap: KnockBack effect applied to %s"),
//						*OtherActor->GetName());
//					break;
//				}
//				case EnumEffectType::Destroy:
//				{
//					//OtherActor->Destroy();
//					UE_LOG(LogTemp, Warning, TEXT(" Destroyed "));
//					//UE_LOG(LogTemp, Warning, TEXT("Special HitBox Overlap: Destroyed %s"), *OtherActor->GetName());
//					break;
//				}
//				default:
//					break;
//				}
//			}
//		}
//	}
//}

void AEntityPreset::OnSpecialHitBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this) return;

	ACharacter* PlayerCharacter = Cast<ACharacter>(OtherActor);
	if (!PlayerCharacter) return;

	// 1. Damage를 먼저 처리
	for (const FSkillEffectData& Effect : SpecialSkillEffectData)
	{
		if (Effect.EffectType == EnumEffectType::Damage)
		{
			float DamageToApply = Effect.EffectValue01;
			UGameplayStatics::ApplyDamage(OtherActor, DamageToApply, GetController(), this, nullptr);
			UE_LOG(LogTemp, Warning, TEXT("Special HitBox Overlap: Applied Damage %f to %s"),
				DamageToApply, *OtherActor->GetName());
		}
	}

	// 2. KnockBack 효과 처리
	for (const FSkillEffectData& Effect : SpecialSkillEffectData)
	{
		if (Effect.EffectType == EnumEffectType::KnockBack)
		{
			// LaunchCharacter 또는 AddImpulse 로 구현
			FVector KnockbackDir = OtherActor->GetActorLocation() - GetActorLocation();
			KnockbackDir.Normalize();
			float KnockbackDistance = Effect.EffectValue01;
			float KnockbackDuration = Effect.EffectValue02;

			if (KnockbackDistance <= 0.01f)
			{
				KnockbackDuration = 0.1f; // 최소 보정
			}

			// 속도 = 거리/시간
			float KnockbackSpeed = KnockbackDistance / KnockbackDuration;

			FVector KnockbackVelocity = KnockbackDir * KnockbackSpeed;

			ACharacter* HitCharacter = Cast<ACharacter>(OtherActor);
			if (HitCharacter)
			{
				UCharacterMovementComponent* MoveComp = HitCharacter->GetCharacterMovement();
				if (MoveComp)
				{
					// 기존 모멘텀 무시하고 새로운 속도로 밀기
					HitCharacter->LaunchCharacter(KnockbackVelocity, true, true);

					// 마찰력 없애서 정확하게 이동
					MoveComp->BrakingFrictionFactor = 0.f;
					MoveComp->GroundFriction = 0.f;

					// 일정 시간 후 마찰력 복원 (안 그러면 계속 미끄러짐)
					FTimerHandle FrictionRestoreHandle;
					FTimerDelegate RestoreFriction;
					RestoreFriction.BindLambda([=]() {
						MoveComp->BrakingFrictionFactor = 2.f;
						MoveComp->GroundFriction = 8.f;
						});
					OtherActor->GetWorldTimerManager().SetTimer(FrictionRestoreHandle, RestoreFriction, KnockbackDuration, false);
				}
			}

			UE_LOG(LogTemp, Warning, TEXT("KnockBack to %s → Distance: %.1fcm in %.2fs (speed: %.1f)"),
				*OtherActor->GetName(), KnockbackDistance, KnockbackDuration, KnockbackSpeed);

			/*UCharacterMovementComponent* MoveComp = PlayerCharacter->GetCharacterMovement();
			if (MoveComp)
			{
				MoveComp->Launch(KnockbackDir * Force);
			}

			UE_LOG(LogTemp, Warning, TEXT("Special HitBox Overlap: KnockBack applied to %s with force %f"), *OtherActor->GetName(), Force);*/
		}
	}

	// 3. Destroy 효과는 마지막에 처리 (원하면 주석 처리 가능)
	for (const FSkillEffectData& Effect : SpecialSkillEffectData)
	{
		if (Effect.EffectType == EnumEffectType::Destroy)
		{
			// 파괴가 정말 필요하다면 여기에
			// OtherActor->Destroy();
			UE_LOG(LogTemp, Warning, TEXT("Special HitBox Overlap: Destroy effect processed (log only) for %s"), *OtherActor->GetName());
		}
	}
}



void AEntityPreset::PerformSkill_Charge()
{
	// 스킬 시전 플래그 설정
	// 해당 변수가 true일 동안엔 다른 스킬 시전 불가능
	bIsCastingSkill = true;

	// 저장된 전방 방향
	ChargeDirection = GetActorForwardVector().GetSafeNormal();
	ChargeStartLocation = GetActorLocation();
	ChargeDistance = SpecialSkillData.SkillRange;
	ChargeTargetLocation = ChargeStartLocation + ChargeDirection * ChargeDistance;

	// AI 경로 추적 중지 
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		if (UPathFollowingComponent* PathComp = AIController->GetPathFollowingComponent())
		{
			// 경로 추적 중단
			PathComp->Deactivate();

			AIController->StopMovement();
			UE_LOG(LogTemp, Warning, TEXT("AI movement forcibly stopped before LaunchCharacter"));
		}
	}
	//// 현재 전방 벡터를 저장 (처음 결정된 방향을 고정)
	//StoredDashDirection = GetActorForwardVector().GetSafeNormal();

	if (SpecialSkillMontage)
	{
		if (UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
		{
			// 스페셜 스킬 몽타주에 플레이어에게 돌진하기 전, 준비 애니메이션(1초) 포함
			AnimInst->Montage_Play(SpecialSkillMontage);
			UE_LOG(LogTemp, Warning, TEXT("PerformSpecialSkill_Charge: Montage played"));

			//// 돌진 경로 표시
			//DrawChargePath();

			// 경로 시각화 (기획 확인용)
			DrawDebugLine(GetWorld(), ChargeStartLocation, ChargeTargetLocation, FColor::Red, false, 2.0f, 0, 3.0f);

			// 몽타주 종료 델리게이트 바인딩 (몽타주 종료 = 스킬 종료)
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AEntityPreset::OnSkillMontageEnded);
			AnimInst->Montage_SetEndDelegate(EndDelegate, SpecialSkillMontage);

			// 준비 시간 후 돌진 실행
			float PrepTime = 1.0f; 
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, this, &AEntityPreset::ExecuteChargeDash, PrepTime, false);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("PerformSpecialSkill_Charge: AnimInstance not found"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PerformSpecialSkill_Charge: SpecialSkillMontage is not set"));
	}
}

void AEntityPreset::ExecuteChargeDash()
{
	float LaunchSpeed = 2000.0f;

	// 실제 돌진
	LaunchCharacter(ChargeDirection * LaunchSpeed, true, true);
	bIsCharging = true;

	// 히트박스도 활성화
	ShowSpecialHitBox();

	UE_LOG(LogTemp, Warning, TEXT("ExecuteChargeDash: Launch Started"));

	//// 준비 시간 동안 저장된 dash 방향을 사용
	//FVector DashDirection = StoredDashDirection;

	//// SkillData에 정의된 SkillRange를 돌진 거리로 사용 
	////float DashDistance = SpecialSkillData.SkillRange;

	////FVector StartLocation = GetActorLocation();
	////FVector DashTarget = StartLocation + DashDirection * DashDistance;

	//// 히트박스 활성화 
	//ShowSpecialHitBox();

	//// 돌진
	//float DashSpeed = 2000.0f; 
	//LaunchCharacter(DashDirection * DashSpeed, true, false);

	//UE_LOG(LogTemp, Warning, TEXT("ExecuteChargeDash: Dashing"));

	// 스킬 종료 처리는 MontageEnded 콜백에서 처리 
}

void AEntityPreset::ClearCastingSkill()
{
	bIsCastingSkill = false;
}

//  스킬 몽타주 종료 콜백
void AEntityPreset::OnSkillMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 스킬 종료 처리 
	bIsCastingSkill = false;

	// AI 경로 추적 활성화
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		if (UPathFollowingComponent* PathComp = AIController->GetPathFollowingComponent())
		{
			// 경로 추적 활성화 
			PathComp->Activate();
		}
	}
	//// 실제 돌진 동작: 빠른 이동 및 히트박스 생성
 //   // 예를 들어: 전방으로 특정 거리를 빠르게 이동 + ShowSpecialHitBox() 호출
 //   
 //   // 돌진 이동 처리 (임의 값, 필요에 따라 수정)
	//FVector ForwardDirection = GetActorForwardVector();
	//float ChargeDistance = 600.f; // SkillData.SkillRange 등 데이터 사용 가능
	//FVector TargetLocation = GetActorLocation() + ForwardDirection * ChargeDistance;

	//// 빠른 이동(예: Teleport or Smooth movement using a timeline)
	//SetActorLocation(TargetLocation);
	//UE_LOG(LogTemp, Warning, TEXT("OnSpecialSkill_ChargeMontageEnded: Moved to Charge target location"));

	// 돌진 동작 시 히트박스 활성화
	//ShowSpecialHitBox();
	//HideSpecialHitBox();
}

//// Special 스킬 몽타주 종료 콜백
//void AEntityPreset::OnSpecialSkillMontageEnded(UAnimMontage* Montage, bool bInterrupted)
//{
//	// 스킬 종료 처리 
//	bIsCastingSkill = false;
//
//	//// 실제 돌진 동작: 빠른 이동 및 히트박스 생성
// //   // 예를 들어: 전방으로 특정 거리를 빠르게 이동 + ShowSpecialHitBox() 호출
// //   
// //   // 돌진 이동 처리 (임의 값, 필요에 따라 수정)
//	//FVector ForwardDirection = GetActorForwardVector();
//	//float ChargeDistance = 600.f; // SkillData.SkillRange 등 데이터 사용 가능
//	//FVector TargetLocation = GetActorLocation() + ForwardDirection * ChargeDistance;
//
//	//// 빠른 이동(예: Teleport or Smooth movement using a timeline)
//	//SetActorLocation(TargetLocation);
//	//UE_LOG(LogTemp, Warning, TEXT("OnSpecialSkill_ChargeMontageEnded: Moved to Charge target location"));
//
//	// 돌진 동작 시 히트박스 활성화
//	//ShowSpecialHitBox();
//	//HideSpecialHitBox();
//}

void AEntityPreset::StopMovement()
{
	GetCharacterMovement()->StopMovementImmediately();
}

void AEntityPreset::DrawChargePath()
{
	// 예시: 디버그 선을 이용해 경로 표시
	FVector StartLocation = GetActorLocation();
	//FVector EndLocation = StartLocation + GetActorForwardVector() * 600.f; // 돌진 거리와 동일
	float DashSpeed = 2000.0f;
	float DashDuration = 0.75f; // 돌진 지속 시간 추정 (0.5~1.0s 정도로 테스트)
	FVector EndLocation = StartLocation + GetActorForwardVector() * DashSpeed * DashDuration;

	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 1.5f, 0, 5.f);
	UE_LOG(LogTemp, Warning, TEXT("DrawChargePath: Charge path drawn"));
}


//void AEntityPreset::OnNormalHitBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	if (OtherActor && OtherActor != this)
//	{
//		// 플레이어 캐릭터인지 확인
//		ACharacter* PlayerCharacter = Cast<ACharacter>(OtherActor);
//		if (PlayerCharacter)
//		{
//			// Normal 스킬 효과에 따른 대미지 적용
//			float DamageToApply = NormalSkillEffectData.EffectValue01;
//
//			UGameplayStatics::ApplyDamage(OtherActor, DamageToApply, GetController(), this, nullptr);
//			UE_LOG(LogTemp, Warning, TEXT("HitBox Overlap: Applied %f damage to %s"), DamageToApply, *OtherActor->GetName());
//
//			// 만약 한 번만 적용하고 히트박스를 파괴하고 싶다면
//			// NormalSkillHitBox->SetHiddenInGame(true); 또는 Destroy();
//		}
//	}
//}
//
//void AEntityPreset::OnSpecialHitBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	if (OtherActor && OtherActor != this)
//	{
//		// 플레이어 캐릭터인지 확인
//		ACharacter* PlayerCharacter = Cast<ACharacter>(OtherActor);
//		if (PlayerCharacter)
//		{
//			// Special 스킬 효과에 따른 대미지 적용
//			float DamageToApply = SpecialSkillEffectData.EffectValue01;
//
//			UGameplayStatics::ApplyDamage(OtherActor, DamageToApply, GetController(), this, nullptr);
//			UE_LOG(LogTemp, Warning, TEXT("HitBox Overlap: Applied %f damage to %s"), DamageToApply, *OtherActor->GetName());
//		}
//	}
//}

EnumAttackType AEntityPreset::GetAttackType()
{
	UE_LOG(LogTemp, Warning, TEXT("Get AttackType: %d"), (uint8)currentAttackType);

	return currentAttackType;
}

float AEntityPreset::GetNormalSkillRange()
{
	UE_LOG(LogTemp, Warning, TEXT("Get Normal Skill Range: %f"), currentNormalSkillRange);

	return currentNormalSkillRange;
}

float AEntityPreset::GetSpecialSkillRange()
{
	UE_LOG(LogTemp, Warning, TEXT("Get Special Skill Range: %f"), currentSpecialSkillRange);

	return currentSpecialSkillRange;
}
//
//void AEntityPreset::PerformNormalSkill()
//{
//	if (NormalSkillMontage)
//	{
//		if (UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
//		{
//			// 몽타주 재생
//			AnimInst->Montage_Play(NormalSkillMontage);
//			UE_LOG(LogTemp, Warning, TEXT("PerformNormalSkill: Montage played"));
//		}
//		else
//		{
//			UE_LOG(LogTemp, Error, TEXT("PerformNormalSkill: AnimInstance not found"));
//		}
//	}
//	else
//	{
//		UE_LOG(LogTemp, Error, TEXT("PerformNormalSkill: NormalSkillMontage is not set"));
//	}
//}
//
//void AEntityPreset::PerformSpecialSkill()
//{
//	if (SpecialSkillMontage)
//	{
//		if (UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
//		{
//			// 몽타주 재생
//			AnimInst->Montage_Play(SpecialSkillMontage);
//			UE_LOG(LogTemp, Warning, TEXT("PerformSpecialSkill: Montage played"));
//
//			// 몽타주 종료 델리게이트 바인딩 
//			FOnMontageEnded EndDelegate;
//			EndDelegate.BindUObject(this, &AEntityPreset::OnSpecialSkillMontageEnded);
//			AnimInst->Montage_SetEndDelegate(EndDelegate, SpecialSkillMontage);
//
//			ShowSpecialHitBox();
//		}
//		else
//		{
//			UE_LOG(LogTemp, Error, TEXT("PerformSpecialSkill: AnimInstance not found"));
//		}
//	}
//	else
//	{
//		UE_LOG(LogTemp, Error, TEXT("PerformSpecialSkill: NormalSkillMontage is not set"));
//	}
//}

