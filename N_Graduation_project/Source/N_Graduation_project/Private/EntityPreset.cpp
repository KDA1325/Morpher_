#include "EntityPreset.h"
#include "EntityWidget.h"
#include "EntitySkillComponent.h"
#include "MyAIController.h"
#include "MyAI.h"
#include "EntityProjectile.h"
#include "N_Graduation_project/N_Graduation_projectCharacter.h"
#include "PlayerSkillComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "WidgetActor.h"
#include "CharacterStateComponent.h"
#include "BrainComponent.h"
#include "Components/SphereComponent.h"


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

	//SkillArrowChildComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("SkillArrowChildComponent"));
	////SkillArrowChildComponent->SetupAttachment(RootComponent); // 또는 RootComponent
	//SkillArrowChildComponent->SetupAttachment(GetMesh(),TEXT("ArrowSocket")); // 또는 RootComponent
	////SkillArrowComponent->SetChildActorClass(AEntityProjectile::StaticClass());

	////TestChildActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("TestChildActor"));
	////TestChildActor->SetupAttachment(RootComponent);  // 대충 붙이기

	////// pitch, yaw, roll: x, z, y
	////FRotator NewRotation = FRotator(0.f,0.f,-90.f);
	////SkillArrowChildComponent->SetRelativeRotation(NewRotation);

}

// Called when the game starts or when spawned
void AEntityPreset::BeginPlay()
{
	Super::BeginPlay();
	// WidgetComponent를 통해 위젯 인스턴스를 가져와 EntityWidget에 할당
	FName WidgetCompName = TEXT("EntityPresetWidget");
	WidgetComp = Cast<UWidgetComponent>(GetDefaultSubobjectByName(WidgetCompName));  // 멤버 변수 WidgetComp 사용

	if(WidgetComp)
	{
		UUserWidget* UserWidget = WidgetComp->GetUserWidgetObject();
		if(UserWidget)
		{
			EntityWidget = Cast<UEntityWidget>(UserWidget);
			if(EntityWidget)
			{
				OnHealthChanged.Broadcast(CurrentHP);

				EntityWidget->EntityPreset = this;
				// 체력 변경 델리게이트 바인딩
				OnHealthChanged.AddDynamic(EntityWidget,&UEntityWidget::UpdateHealthBar);
				UE_LOG(LogTemp,Warning,TEXT("banana HP Bar connected %s"),*GetName());
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
	if(!NormalHitBoxContainer)
	{
		NormalHitBoxContainer = NewObject<USceneComponent>(this,TEXT("NormalHitBoxContainer"));
		if(NormalHitBoxContainer)
		{
			// 컨테이너를 컴포넌트로 등록 
			NormalHitBoxContainer->RegisterComponent();

			// 스켈레탈 메시의 소켓에 부착
			// 컨테이너는 소켓의 원점을 그대로 유지 (즉, 히트박스가 소켓 위치에서 시작)
			NormalHitBoxContainer->AttachToComponent(GetMesh(),FAttachmentTransformRules::KeepRelativeTransform,TEXT("BiteHitBox"));
		}
	}
	if(!SpecialHitBoxContainer)
	{
		SpecialHitBoxContainer = NewObject<USceneComponent>(this,TEXT("SpecialHitBoxContainer"));
		if(SpecialHitBoxContainer)
		{
			// 컨테이너를 컴포넌트로 등록 
			SpecialHitBoxContainer->RegisterComponent();

			// 스켈레탈 메시의 소켓에 부착
			// 컨테이너는 소켓의 원점을 그대로 유지 (즉, 히트박스가 소켓 위치에서 시작)
			SpecialHitBoxContainer->AttachToComponent(GetMesh(),FAttachmentTransformRules::KeepRelativeTransform,TEXT("ChargeHitBox"));
		}
	}

	if(ChargeCurve)
	{
		ChargeTimeline = NewObject<UTimelineComponent>(this,FName("WildBoar_ChargeFloat"));
		ChargeTimeline->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		ChargeTimeline->SetNetAddressable();
		ChargeTimeline->SetPropertySetObject(this);
		//ChargeTimeline->SetDirectionPropertyName(FName("ChargeTimelineDirection"));
		ChargeTimeline->SetLooping(false);
		ChargeTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);

		FOnTimelineFloat ProgressFunction;
		ProgressFunction.BindUFunction(this,FName("Timeline_ChargeProgress"));
		ChargeTimeline->AddInterpFloat(ChargeCurve,ProgressFunction,FName("WildBoar_ChargeFloat"));

		//FOnTimelineEvent FinishedFunction;
		//FinishedFunction.BindUFunction(this, FName("Timeline_ChargeFinished"));
		//ChargeTimeline->SetTimelineFinishedFunc(FinishedFunction);

		ChargeTimeline->RegisterComponent();
	}

	//if(SkillArrowChildComponent)
	//{
	//	SkillArrowChildComponent->SetRelativeRotation(FRotator(0.f,-90.f,0.f)); // 필요시 회전
	//}
	//if(SkillArrowComponent)
	//{
	//	

	//	UE_LOG(LogTemp,Warning,TEXT("Arrow rotated 180 degrees on Z-axis."));

	//	
	//}

	//// 화살 회전 보정
	//if(SkillArrowChildComponent)
	//{
	//	SkillArrowChildComponent->SetRelativeRotation(FRotator(0.f,90.f,0.f));
	//	UE_LOG(LogTemp,Warning,TEXT("SkillArrowChildComponent 보정 완료"));


	//	Arrow = Cast<AEntityProjectile>(SkillArrowChildComponent->GetChildActor());

	//	SkillArrowChildComponent->SetVisibility(false);
	//	SkillArrowChildComponent->SetHiddenInGame(true);
	//}

	this->bIsDefending = false;
}

// Called every frame
void AEntityPreset::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bIsCharging)
	{
		float TravelledDistance = FVector::Dist(ChargeStartLocation,GetActorLocation());
		if(TravelledDistance >= ChargeDistance)
		{
			StopMovement();
			bIsCharging = false;

			HideSpecialHitBox(); // 돌진 끝났으면 히트박스 제거
			UE_LOG(LogTemp,Warning,TEXT("Charge Finished — Distance Reached"));
		}
	}
}

// Called to bind functionality to input
void AEntityPreset::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float AEntityPreset::TakeDamage(float DamageAmount,FDamageEvent const& DamageEvent,AController* EventInstigator,AActor* DamageCauser)
{
	// 방어 중
	if(this->bIsDefending)
	{
		UE_LOG(LogTemp,Warning,TEXT("[TakeDamage] Monster %s (%p) is defending."),*GetName(),this);

		// 대미지 원인이 플레이어 캐릭터 스킬인지 확인
		AN_Graduation_projectCharacter* PlayerCharacter = Cast<AN_Graduation_projectCharacter>(DamageCauser);
		//ACharacter* PlayerCharacter = Cast<ACharacter>(DamageCauser);
		UE_LOG(LogTemp,Warning,TEXT("DamageCauser: %s (%s)"),*GetNameSafe(DamageCauser),DamageCauser ? *DamageCauser->GetClass()->GetName() : TEXT("nullptr"));

		UPlayerSkillComponent* PlayerSkillComponent = PlayerCharacter->FindComponentByClass<UPlayerSkillComponent>();

		// 플레이어 캐릭터 스킬이 !bIsSpecialAttack = 노멀 스킬이라면
		if(PlayerSkillComponent)
		{
			// 플레이어가 노말 스킬 공격을 했을 때
			if(!PlayerSkillComponent->bIsSpecialAttack)
			{
				UE_LOG(LogTemp,Warning,TEXT("[Defending]: Ignore Normal Skill Damage"));

				return 0.0f;
			} else
			{
				UE_LOG(LogTemp,Error,TEXT("[Defending]: Take Special Attack Damage"));
			}
		} else
		{
			UE_LOG(LogTemp,Error,TEXT("PlayerSkillComponent is NULL"));
		}
	}

	UE_LOG(LogTemp,Log,TEXT("banana Damage: %f"),DamageAmount);
	float ActualDamage = Super::TakeDamage(DamageAmount,DamageEvent,EventInstigator,DamageCauser); // 부모 클래스의 TakeDamage 호출
	UE_LOG(LogTemp,Log,TEXT("banana (TakeDamage)CurrentHP: %f, DamageAmount: %f, CurrentHP - DamageAmount: %f"),CurrentHP,DamageAmount,CurrentHP - DamageAmount);

	if(CurrentHP > 0)
	{
		ApplyDamage(DamageAmount);
	}

	return ActualDamage;
}

void AEntityPreset::SetHP(float NewHP)
{
	CurrentHP = FMath::Clamp(NewHP,0.0f,MaxHp);
	//CurrentHP = NewHP;
	// 델리게이트 호출
	//UE_LOG(LogTemp, Warning, TEXT("banana SetHP - NewHP: %f, ClampedHP: %f, MaxHP: %f"), NewHP, CurrentHP, MaxHp);
	OnHealthChanged.Broadcast(CurrentHP);

	if(CurrentHP <= 0)
	{
		if(APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(),0))
		{
			UE_LOG(LogTemp,Warning,TEXT("yorimo PlayerController 존재"));

			APawn* PlayerPawn = PC->GetPawn();
			ACharacter* Character = Cast<ACharacter>(PlayerPawn);

			// 여기가 핵심: 플레이어에서 WidgetActor 찾기
			if(UWidgetActor* WidgetActor = Character->FindComponentByClass<UWidgetActor>())
			{
				UE_LOG(LogTemp,Warning,TEXT("yorimo WidgetActor 찾음"));
				if(!WidgetActor->PieWidget->DeadMonsters.Contains(currentPreset))
				{
					WidgetActor->HUDWidget->OnCollection_Implementation(currentPreset);
					WidgetActor->PieWidget->DeadMonsters.Add(currentPreset);
					UE_LOG(LogTemp,Warning,TEXT("yorimo DeadMonsters에 %s 추가됨"),*currentPreset);
					WidgetActor->PieWidget->OpenCharacter(currentPreset);
				} else {
					UE_LOG(LogTemp,Warning,TEXT("yorimo 추가실패"));

				}

			}
		}
		UE_LOG(LogTemp,Warning,TEXT("banana Entity Die"));
		Destroy();
	}

}

void AEntityPreset::ApplyDamage(float DamageAmount)
{
	SetHP(CurrentHP - DamageAmount);
}

float AEntityPreset::GetHPRatio()
{
	if(MaxHp > 0)
	{
		//UE_LOG(LogTemp, Log, TEXT("banana MaxHp > 0"));
		return(CurrentHP / MaxHp);
	} else {
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
	///SetActorLabel(InEntityData.EntityName);
	SetMoveSpeed(InEntityData.MoveSpeed);
	SetHP(InEntityData.HP);
	currentPreset = InEntityData.EntityGroupID;

	UE_LOG(LogTemp,Error,TEXT("banana Initialized Entity with Name: %s, HP: %d, Move Speed: %d"),
		*InEntityData.EntityName,InEntityData.HP,InEntityData.MoveSpeed);

	// EntityData에 저장된 Normal Skill 식별자를 통해 스킬 데이터 가져옴 
	if(UABGameSingleton::Get().GetSkillDataBySkillID(InEntityData.NormalSkill,NormalSkillData))
	{
		UE_LOG(LogTemp,Warning,TEXT("Loaded Skill Data: %s, Skill Type: %d, Skill Range: %f"),
			*NormalSkillData.SkillNameID,(uint8)NormalSkillData.SkillType,NormalSkillData.SkillRange);

		NormalSkillID = InEntityData.NormalSkill;

		SetNormalSkillRange(NormalSkillData.SkillRange);

		// SkillType이 HitBox라면 
		if(NormalSkillData.SkillType == EnumSkillType::HitBox)
		{
			SetupHitBoxComponent(NormalSkillData);
		}

		// NormalSkillData에 저장된 SkillNameID 식별자를 통해 스킬 효과 데이터 가져옴
		if(UABGameSingleton::Get().GetSkillEffectDataBySkillID(NormalSkillData.SkillNameID,NormalSkillEffectData))
		{
			for(const FSkillEffectData& EffectData : NormalSkillEffectData)
			{
				UE_LOG(LogTemp,Warning,TEXT("Loaded Skill Effect Data: %s, Effect Type: %d, Effect Value01: %f, Effect Value02: %f"),
					*EffectData.SkillNameID,(uint8)EffectData.EffectType,EffectData.EffectValue01,EffectData.EffectValue02);
			}
		}
	}

	// EntityData에 저장된 Special Skill 식별자를 통해 스킬 데이터 가져옴 
	if(UABGameSingleton::Get().GetSkillDataBySkillID(InEntityData.SpecialSkill,SpecialSkillData))
	{
		UE_LOG(LogTemp,Warning,TEXT("Loaded Skill Data: %s, Skill Type: %d, Skill Range: %f"),
			*SpecialSkillData.SkillNameID,(uint8)SpecialSkillData.SkillType,SpecialSkillData.SkillRange);

		SpecialSkillID = InEntityData.SpecialSkill;

		SetSpecialSkillRange(SpecialSkillData.SkillRange);

		// SkillType이 HitBox라면 
		if(SpecialSkillData.SkillType == EnumSkillType::HitBox)
		{
			SetupHitBoxComponent(SpecialSkillData);
		}

		// SpecialSkillData에 저장된 SkillNameID 식별자를 통해 스킬 효과 데이터 가져옴
		if(UABGameSingleton::Get().GetSkillEffectDataBySkillID(SpecialSkillData.SkillNameID,SpecialSkillEffectData))
		{
			for(const FSkillEffectData& EffectData : SpecialSkillEffectData)
			{

				UE_LOG(LogTemp,Warning,TEXT("Loaded Skill Effect Data: %s, Effect Type: %d, Effect Value01: %f, Effect Value02: %f"),
					*EffectData.SkillNameID,(uint8)EffectData.EffectType,EffectData.EffectValue01,EffectData.EffectValue02);
			}
		}
	}
}

void AEntityPreset::SetupHitBoxComponent(FSkillData& SkillData)
{
	if(SkillData.SkillTypeShape == EnumSkillTypeShape::Box)
	{
		// SkillNameID -> 소켓 이름 매핑 테이블
		TMap<FString,FName> SkillToSocketMap = {
			//{ "Skill_Slash", TEXT("SlashSocket") },
			{"Skill_Bite",TEXT("BiteHitBox")},
			{"Skill_Charge",TEXT("ChargeHitBox")},
			{"Skill_TailSwing",TEXT("TailSocket")},
			{"Skill_FreezeBreath",TEXT("BreathSocket")},
			{"Skill_ArmSwing",TEXT("ArmSocket")},
			{"Skill_SkeletonSlash",TEXT("SlashSocket")}
		};

		FName SocketToAttach = TEXT("DefaultHitBox");
		if(FName* FoundSocket = SkillToSocketMap.Find(SkillData.SkillNameID))
		{
			SocketToAttach = *FoundSocket;
		}

		USceneComponent* Container = nullptr;
		if(SkillData.SkillNameID == "Skill_Charge")
		{
			Container = SpecialHitBoxContainer;
		} else
		{
			Container = NormalHitBoxContainer;
		}

		Container->AttachToComponent(GetMesh(),FAttachmentTransformRules::KeepRelativeTransform,SocketToAttach);

		// 스킬 타입에 따라 해당 히트박스 컴포넌트 생성
		if(SkillData.SkillNameID == "Skill_Charge")
		{
			// Special 히트박스 생성
			if(!SpecialSkillHitBox)
			{
				SpecialSkillHitBox = NewObject<UBoxComponent>(this,TEXT("SpecialSkillHitBox"));
				if(SpecialSkillHitBox)
				{
					SpecialSkillHitBox->RegisterComponent();
					SpecialSkillHitBox->AttachToComponent(Container,FAttachmentTransformRules::KeepRelativeTransform);
					HideSpecialHitBox();

					ConfigureHitBox(SpecialSkillHitBox);
					SpecialSkillHitBox->OnComponentBeginOverlap.AddDynamic(this,&AEntityPreset::OnSpecialHitBoxOverlap);
				}
			}

			// UBoxComponent는 half extents를 사용
				// 전방 길이로 SkillTypeSizeX를 전체 길이로 보고, 이 값을 절반으로 해서 half extent로 사용
				// half extent의 X축 값은 히트박스의 Y 크기 값(SkillTypeSizeY/2),
				// Y축은 Z 크기 값,
				// Z축은 X 크기 값(SkillTypeSizeX/2)
			FVector HalfExtent = FVector(SkillData.SkillTypeSizeY / 2.0f,50.0f,SkillData.SkillTypeSizeX / 2.0f);
			SpecialSkillHitBox->SetBoxExtent(HalfExtent);

			// 기본 UBoxComponent는 자신의 중심을 기준으로 확장
			// 기획서에 따라 소켓(HitBoxContainer)의 원점에서부터 전방(X축)으로만 확장되도록 하기
			// UBoxComponent를 HitBoxContainer의 자식으로 두고, 상대 위치를 Z축(+X 방향)으로 half extent만큼 이동
			FVector NewRelativeLocation = FVector(0.0f,0.0f,HalfExtent.X);
			SpecialSkillHitBox->SetRelativeLocation(NewRelativeLocation);

			UE_LOG(LogTemp,Warning,TEXT("SetupSpecialHitBoxComponent: HalfExtent=%s, NewRelativeLocation=%s"),
				*HalfExtent.ToString(),*NewRelativeLocation.ToString());
		} else if(SkillData.SkillNameID == "Skill_FreezeBreath")
		{
			// Special 히트박스 생성
			if(!SpecialSkillHitBox)
			{
				SpecialSkillHitBox = NewObject<UBoxComponent>(this,TEXT("SpecialSkillHitBox"));
				if(SpecialSkillHitBox)
				{
					SpecialSkillHitBox->RegisterComponent();
					SpecialSkillHitBox->AttachToComponent(Container,FAttachmentTransformRules::KeepRelativeTransform);
					HideSpecialHitBox();

					ConfigureHitBox(SpecialSkillHitBox);
					SpecialSkillHitBox->OnComponentBeginOverlap.AddDynamic(this,&AEntityPreset::OnSpecialHitBoxOverlap);
				}
			}

			// UBoxComponent는 half extents를 사용
				// 전방 길이로 SkillTypeSizeX를 전체 길이로 보고, 이 값을 절반으로 해서 half extent로 사용
				// half extent의 X축 값은 히트박스의 Y 크기 값(SkillTypeSizeY/2),
				// Y축은 Z 크기 값,
				// Z축은 X 크기 값(SkillTypeSizeX/2)
			FVector HalfExtent = FVector(SkillData.SkillTypeSizeY / 2.0f,50.0f,SkillData.SkillTypeSizeX / 2.0f);
			SpecialSkillHitBox->SetBoxExtent(HalfExtent);

			// 기본 UBoxComponent는 자신의 중심을 기준으로 확장
			// 기획서에 따라 소켓(HitBoxContainer)의 원점에서부터 전방(X축)으로만 확장되도록 하기
			// UBoxComponent를 HitBoxContainer의 자식으로 두고, 상대 위치를 Z축(+X 방향)으로 half extent만큼 이동
			FVector NewRelativeLocation = FVector(0.0f,0.0f,HalfExtent.X);
			SpecialSkillHitBox->SetRelativeLocation(NewRelativeLocation);

			UE_LOG(LogTemp,Warning,TEXT("SetupSpecialHitBoxComponent: HalfExtent=%s, NewRelativeLocation=%s"),
				*HalfExtent.ToString(),*NewRelativeLocation.ToString());
		} else
		{
			// Normal 히트박스 생성
			if(!NormalSkillHitBox)
			{
				NormalSkillHitBox = NewObject<UBoxComponent>(this,TEXT("NormalSkillHitBox"));
				if(NormalSkillHitBox)
				{
					NormalSkillHitBox->RegisterComponent();
					NormalSkillHitBox->AttachToComponent(NormalHitBoxContainer,FAttachmentTransformRules::KeepRelativeTransform);

					HideNormalHitBox();

					ConfigureHitBox(NormalSkillHitBox);

					// Overlap 이벤트 바인딩 
					NormalSkillHitBox->OnComponentBeginOverlap.AddDynamic(this,&AEntityPreset::OnNormalHitBoxOverlap);
				}
			}

			// UBoxComponent는 half extents를 사용
				// 전방 길이로 SkillTypeSizeX를 전체 길이로 보고, 이 값을 절반으로 해서 half extent로 사용
				// half extent의 X축 값은 히트박스의 Y 크기 값(SkillTypeSizeY/2),
				// Y축은 Z 크기 값,
				// Z축은 X 크기 값(SkillTypeSizeX/2)
			FVector HalfExtent = FVector(SkillData.SkillTypeSizeY / 2.0f,50.0f,SkillData.SkillTypeSizeX / 2.0f);
			NormalSkillHitBox->SetBoxExtent(HalfExtent);

			// 기본 UBoxComponent는 자신의 중심을 기준으로 확장
			// 기획서에 따라 소켓(HitBoxContainer)의 원점에서부터 전방(X축)으로만 확장되도록 하기
			// UBoxComponent를 HitBoxContainer의 자식으로 두고, 상대 위치를 Z축(+X 방향)으로 half extent만큼 이동
			FVector NewRelativeLocation = FVector(0.0f,0.0f,HalfExtent.X);
			NormalSkillHitBox->SetRelativeLocation(NewRelativeLocation);

			UE_LOG(LogTemp,Warning,TEXT("SetupNormalHitBoxComponent: HalfExtent=%s, NewRelativeLocation=%s"),
				*HalfExtent.ToString(),*NewRelativeLocation.ToString());
		}
	}

	if(SkillData.SkillTypeShape == EnumSkillTypeShape::Sphere)
	{
		UE_LOG(LogTemp,Warning,TEXT("Sphere"));

		// SkillNameID -> 소켓 이름 매핑 테이블
		TMap<FString,FName> SkillToSocketMap = {
			{"Skill_EarthBreaker",TEXT("BreakerSocket")}
		};

		FName SocketToAttach = TEXT("DefaultHitBox");
		if(FName* FoundSocket = SkillToSocketMap.Find(SkillData.SkillNameID))
		{
			SocketToAttach = *FoundSocket;
		}

		USceneComponent* Container = nullptr;

		if(SkillData.SkillNameID == "Skill_EarthBreaker")
		{
			Container = SpecialHitBoxContainer;
		} else
		{
			Container = NormalHitBoxContainer;
		}

		Container->AttachToComponent(GetMesh(),FAttachmentTransformRules::KeepRelativeTransform,SocketToAttach);

		// 스킬 타입에 따라 해당 히트박스 컴포넌트 생성
		if(SkillData.SkillNameID == "Skill_EarthBreaker")
		{
			// Special 히트박스 생성
			if(!SpecialSkillSphereHitBox)
			{
				SpecialSkillSphereHitBox = NewObject<USphereComponent>(this,TEXT("SpecialSkillSphereHitBox"));
				if(SpecialSkillSphereHitBox)
				{
					SpecialSkillSphereHitBox->RegisterComponent();
					SpecialSkillSphereHitBox->AttachToComponent(Container,FAttachmentTransformRules::KeepRelativeTransform);
					HideSpecialSphereHitBox();

					ConfigureSphereHitBox(SpecialSkillSphereHitBox);
					SpecialSkillSphereHitBox->OnComponentBeginOverlap.AddDynamic(this,&AEntityPreset::OnSpecialHitBoxOverlap);
				}
			}

			// 전방 길이로 SkillTypeSizeX를 전체 길이로 보고, 이 값을 절반으로 해서 HalfRadius로 사용
			// HalfRadiust의 X축 값은 히트박스의 Y 크기 값(SkillTypeSizeY/2),
			// Y축은 Z 크기 값,
			// Z축은 X 크기 값(SkillTypeSizeX/2)
			//float HalfRadius = float(SkillData.SkillTypeSizeX / 2.0f);
			float Radius = SkillData.SkillTypeSizeX;
			SpecialSkillSphereHitBox->SetSphereRadius(Radius);

			// 기본 USphereComponent는 자신의 중심을 기준으로 확장
			// 기획서에 따라 소켓(HitBoxContainer)의 원점에서부터 확장되도록 하기
			// USphereComponent를 HitBoxContainer의 자식으로 두고, 상대 위치를 Z축(+X 방향)으로 HalfRadiust만큼 이동
			//FVector NewRelativeLocation = FVector(SkillData.SkillTypeSizeX);
			FVector NewRelativeLocation = FVector::ZeroVector;
			SpecialSkillSphereHitBox->SetRelativeLocation(NewRelativeLocation);

			UE_LOG(LogTemp,Warning,TEXT("SetupSpecialSkillSphereHitBox: HalfRadius=%.1f, NewRelativeLocation=%s"),
				Radius,*NewRelativeLocation.ToString());
		} else
		{
			// Normal 히트박스 생성
			if(!NormalSkillHitBox)
			{
				NormalSkillHitBox = NewObject<UBoxComponent>(this,TEXT("NormalSkillHitBox"));
				if(NormalSkillHitBox)
				{
					NormalSkillHitBox->RegisterComponent();
					NormalSkillHitBox->AttachToComponent(NormalHitBoxContainer,FAttachmentTransformRules::KeepRelativeTransform);

					HideNormalHitBox();

					ConfigureHitBox(NormalSkillHitBox);

					// Overlap 이벤트 바인딩 
					NormalSkillHitBox->OnComponentBeginOverlap.AddDynamic(this,&AEntityPreset::OnNormalHitBoxOverlap);
				}
			}

			// UBoxComponent는 half extents를 사용
				// 전방 길이로 SkillTypeSizeX를 전체 길이로 보고, 이 값을 절반으로 해서 half extent로 사용
				// half extent의 X축 값은 히트박스의 Y 크기 값(SkillTypeSizeY/2),
				// Y축은 Z 크기 값,
				// Z축은 X 크기 값(SkillTypeSizeX/2)
			FVector HalfExtent = FVector(SkillData.SkillTypeSizeY / 2.0f,50.0f,SkillData.SkillTypeSizeX / 2.0f);
			NormalSkillHitBox->SetBoxExtent(HalfExtent);

			// 기본 UBoxComponent는 자신의 중심을 기준으로 확장
			// 기획서에 따라 소켓(HitBoxContainer)의 원점에서부터 전방(X축)으로만 확장되도록 하기
			// UBoxComponent를 HitBoxContainer의 자식으로 두고, 상대 위치를 Z축(+X 방향)으로 half extent만큼 이동
			FVector NewRelativeLocation = FVector(0.0f,0.0f,HalfExtent.X);
			NormalSkillHitBox->SetRelativeLocation(NewRelativeLocation);

			UE_LOG(LogTemp,Warning,TEXT("SetupNormalHitBoxComponent: HalfExtent=%s, NewRelativeLocation=%s"),
				*HalfExtent.ToString(),*NewRelativeLocation.ToString());
		}

	}

	if(NormalSkillHitBox)
	{
		NormalSkillHitBox->OnComponentBeginOverlap.RemoveAll(this);
		NormalSkillHitBox->OnComponentBeginOverlap.AddDynamic(this,&AEntityPreset::OnNormalHitBoxOverlap);
	}

	if(SpecialSkillHitBox)
	{
		SpecialSkillHitBox->OnComponentBeginOverlap.RemoveAll(this); // 중복 방지
		SpecialSkillHitBox->OnComponentBeginOverlap.AddDynamic(this,&AEntityPreset::OnSpecialHitBoxOverlap);
	}
}

// 히트박스 충돌 세팅 
void AEntityPreset::ConfigureHitBox(UBoxComponent* HitBox)
{
	if(!HitBox)
	{
		return;
	}

	HitBox->SetCollisionObjectType(ECC_WorldDynamic);
	HitBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	HitBox->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);
	HitBox->SetGenerateOverlapEvents(true);
}

// 히트박스 충돌 세팅 
void AEntityPreset::ConfigureSphereHitBox(USphereComponent* HitBox)
{
	if(!HitBox)
	{
		return;
	}

	HitBox->SetCollisionObjectType(ECC_WorldDynamic);
	HitBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	HitBox->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);
	HitBox->SetGenerateOverlapEvents(true);
}

void AEntityPreset::ShowNormalHitBox()
{
	float Duration = NormalSkillData.SkillDuration;

	// HitBox 활성화 
	NormalSkillHitBox->SetHiddenInGame(false);
	NormalSkillHitBox->SetVisibility(true);
	NormalSkillHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);  // 충돌 켜기, 물리는 무시하지만 쿼리만 처리
	UE_LOG(LogTemp,Warning,TEXT("Show HitBox"));

	// 유지 시간 이후 HideHitBox 함수 호출  
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle,this,&AEntityPreset::HideNormalHitBox,Duration,false);
}

void AEntityPreset::HideNormalHitBox()
{
	// HitBox 비활성화 
	NormalSkillHitBox->SetHiddenInGame(true);
	//NormalSkillHitBox->SetVisibility(false);
	NormalSkillHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);  // 충돌 끄기
	UE_LOG(LogTemp,Warning,TEXT("Hide HitBox"));
}

void AEntityPreset::ShowSpecialHitBox()
{
	float Duration = SpecialSkillData.SkillDuration;

	// HitBox 활성화 
	SpecialSkillHitBox->SetHiddenInGame(false);
	SpecialSkillHitBox->SetVisibility(true);
	SpecialSkillHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);  // 충돌 켜기
	UE_LOG(LogTemp,Warning,TEXT("Show HitBox"));

	// 유지 시간 이후 HideHitBox 함수 호출  
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle,this,&AEntityPreset::HideSpecialHitBox,Duration,false);
}

void AEntityPreset::ShowSpecialSphereHitBox()
{
	float Duration = SpecialSkillData.SkillDuration;

	// HitBox 활성화 
	SpecialSkillSphereHitBox->SetHiddenInGame(false);
	SpecialSkillSphereHitBox->SetVisibility(true);
	SpecialSkillSphereHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);  // 충돌 켜기
	UE_LOG(LogTemp,Warning,TEXT("Show Sphere HitBox"));

	// 유지 시간 이후 HideHitBox 함수 호출  
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle,this,&AEntityPreset::HideSpecialSphereHitBox,Duration,false);
}

void AEntityPreset::HideSpecialHitBox()
{
	// HitBox 비활성화 
	SpecialSkillHitBox->SetHiddenInGame(true);
	SpecialSkillHitBox->SetVisibility(false);
	SpecialSkillHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);  // 충돌 끄기
	UE_LOG(LogTemp,Warning,TEXT("Hide HitBox"));
}

void AEntityPreset::HideSpecialSphereHitBox()
{
	// HitBox 비활성화 
	SpecialSkillSphereHitBox->SetHiddenInGame(true);
	//SpecialSkillSphereHitBox->SetVisibility(false);
	SpecialSkillSphereHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);  // 충돌 끄기
	UE_LOG(LogTemp,Warning,TEXT("Hide HitBox"));
}

void AEntityPreset::ShowHitBox()
{
	if(bIsCharging || bIsFreezing)
	{
		ShowSpecialHitBox();
	} else if(bIsBreaking)
	{
		UE_LOG(LogTemp,Warning,TEXT("[ShowHitBox] Calling ShowSpecialSphereHitBox (Break)"));
		ShowSpecialSphereHitBox();
	} else
	{
		ShowNormalHitBox();
	}
}

void AEntityPreset::OnNormalHitBoxOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult)
{
	if(OtherActor && OtherActor != this)
	{
		// 플레이어 캐릭터인지 확인
		AN_Graduation_projectCharacter* PlayerCharacter = Cast<AN_Graduation_projectCharacter>(OtherActor);
		if(PlayerCharacter)
		{
			// Normal 스킬에 부여된 모든 효과들을 반복 처리합니다.
			for(const FSkillEffectData& Effect : NormalSkillEffectData)
			{
				switch(Effect.EffectType)
				{
				case EnumEffectType::Damage:
				{
					float DamageToApply = Effect.EffectValue01;
					UGameplayStatics::ApplyDamage(OtherActor,DamageToApply,GetController(),this,nullptr);
					UE_LOG(LogTemp,Warning,TEXT("Normal HitBox Overlap: Applied Damage %f to %s"),
						DamageToApply,*OtherActor->GetName());
					break;
				}
				case EnumEffectType::KnockBack:
				{
					// KnockBack 효과 로직 구현 (예: OtherActor에게 일정 힘을 가해 뒤로 밀어냄)
					UE_LOG(LogTemp,Warning,TEXT("Normal HitBox Overlap: KnockBack effect applied to %s"),
						*OtherActor->GetName());
					break;
				}
				case EnumEffectType::Destroy:
				{
					// Destroy 효과가 적용되는 경우 OtherActor를 파괴하도록 합니다.
					//OtherActor->Destroy();
					UE_LOG(LogTemp,Warning,TEXT("Normal HitBox Overlap: Destroyed %s"),
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

void AEntityPreset::OnSpecialHitBoxOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult)
{
	if(!OtherActor || OtherActor == this) return;

	AN_Graduation_projectCharacter* PlayerCharacter = Cast<AN_Graduation_projectCharacter>(OtherActor);
	if(!PlayerCharacter) return;

	for(const FSkillEffectData& Effect : SpecialSkillEffectData)
	{
		if(OtherActor->ActorHasTag("Player"))
		{
			// 1. Damage 처리
			if(Effect.EffectType == EnumEffectType::Damage)
			{
				float DamageToApply = Effect.EffectValue01;
				UGameplayStatics::ApplyDamage(OtherActor,DamageToApply,GetController(),this,nullptr);
				UE_LOG(LogTemp,Warning,TEXT("Special HitBox Overlap: Applied Damage %f to %s"),
					DamageToApply,*OtherActor->GetName());
			}

			// 2. KnockBack 처리
			if(Effect.EffectType == EnumEffectType::KnockBack)
			{
				float KnockbackDistance = Effect.EffectValue01;
				float KnockbackDuration = Effect.EffectValue02;

				ApplyKnockbackEffect(PlayerCharacter,KnockbackDistance,KnockbackDuration);

				/*UCharacterMovementComponent* MoveComp = PlayerCharacter->GetCharacterMovement();
				if (MoveComp)
				{
					MoveComp->Launch(KnockbackDir * Force);
				}

				UE_LOG(LogTemp, Warning, TEXT("Special HitBox Overlap: KnockBack applied to %s with force %f"), *OtherActor->GetName(), Force);*/
			}

			// Freezing 효과
			if(Effect.EffectType == EnumEffectType::Freezing)
			{
				UE_LOG(LogTemp,Warning,TEXT("Special HitBox Overlap: Freezing effect processed (log only) for %s"),*OtherActor->GetName());

				float Duration = Effect.EffectValue01;
				float SlowFactor = Effect.EffectValue02;

				if(Duration > 0.f && SlowFactor > 0.f)
				{
					UCharacterMovementComponent* MoveComp = PlayerCharacter->GetCharacterMovement();
					if(MoveComp)
					{
						float OriginalSpeed = MoveComp->MaxWalkSpeed;
						float NewSpeed = OriginalSpeed / SlowFactor;

						MoveComp->MaxWalkSpeed = NewSpeed;

						UE_LOG(LogTemp,Warning,TEXT("Freezing applied to %s → NewSpeed: %.1f for %.1f seconds"),*PlayerCharacter->GetName(),NewSpeed,Duration);

						// 일정 시간 뒤에 원래 속도로 복원
						FTimerHandle RestoreHandle;
						FTimerDelegate RestoreDelegate;

						// 캡처값 반드시 복사
						RestoreDelegate.BindLambda([=]() {
							if(PlayerCharacter && PlayerCharacter->GetCharacterMovement())
							{
								PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = OriginalSpeed;
								UE_LOG(LogTemp,Warning,TEXT("Freezing ended: Restored speed %.1f to %s"),OriginalSpeed,*PlayerCharacter->GetName());
							}
						});

						GetWorld()->GetTimerManager().SetTimer(RestoreHandle,RestoreDelegate,Duration,false);
					}
				}
			}

			// Stun 효과
			if(Effect.EffectType == EnumEffectType::Stun)
			{
				UE_LOG(LogTemp,Warning,TEXT("Special HitBox Overlap: Stun effect processed (log only) for %s"),*OtherActor->GetName());

				float Duration = Effect.EffectValue01;

				if(Duration > 0.f)
				{
					UCharacterMovementComponent* MoveComp = PlayerCharacter->GetCharacterMovement();
					if(MoveComp)
					{
						// 스턴 상태로 변경 
						//PlayerCharacter->CharacterStateComponent->isStunned = true;

						// 이동 불가능 
						PlayerCharacter->bCanMove = false;

						UE_LOG(LogTemp,Warning,TEXT("Stun applied to %s for %.1f seconds"),*PlayerCharacter->GetName(),Duration);

						// 일정 시간 뒤에 원래 속도로 복원
						FTimerHandle RestoreHandle;
						FTimerDelegate RestoreDelegate;

						// 캡처값 복사
						RestoreDelegate.BindLambda([=]() {
							if(PlayerCharacter && PlayerCharacter->CharacterStateComponent)
							{
								PlayerCharacter->CharacterStateComponent->isStunned = false;
								PlayerCharacter->bCanMove = true;

								UE_LOG(LogTemp,Warning,TEXT("Stun ended for %s"),*PlayerCharacter->GetName());
							}
						});
						GetWorld()->GetTimerManager().SetTimer(RestoreHandle,RestoreDelegate,Duration,false);

						// 이동 불가능
						//MoveComp->DisableMovement();

				/*
						RestoreDelegate.BindLambda([=]() {
							if(PlayerCharacter && PlayerCharacter->GetCharacterMovement())
							{
								PlayerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
								PlayerCharacter->CustomTimeDilation = 1.f;

								UE_LOG(LogTemp,Warning,TEXT("Stun ended: %s can move"), *PlayerCharacter->GetName());
							}
						});*/

					}
				}
			}

			//Destroy 효과
			if(Effect.EffectType == EnumEffectType::Destroy)
			{
				// 파괴가 필요하다면 여기에
				// OtherActor->Destroy();
				UE_LOG(LogTemp,Warning,TEXT("Special HitBox Overlap: Destroy effect processed (log only) for %s"),*OtherActor->GetName());
			}
		}

	}
}



void AEntityPreset::PerformSkill_Charge()
{
	if(!SpecialSkillMontage) return;

	// 스킬 시전 플래그 설정
	// 해당 변수가 true일 동안엔 다른 스킬 시전 불가능
	bIsCastingSkill = true;

	ChargeDirection = GetActorForwardVector().GetSafeNormal();
	ChargeStartLocation = GetActorLocation();
	ChargeDistance = SpecialSkillData.SkillRange;
	ChargeTargetLocation = ChargeStartLocation + ChargeDirection * ChargeDistance;
	/*ChargeStartLocation = GetActorLocation();
	ChargeDirection = GetActorForwardVector().GetSafeNormal();
	ChargeTargetLocation = ChargeStartLocation + ChargeDirection * SpecialSkillData.SkillRange;*/

	// 인디케이터 표시
	SpawnChargeIndicator(ChargeStartLocation,ChargeTargetLocation);

	// 1초 뒤 데칼 제거
	FTimerHandle DecalTimerHandle;
	GetWorldTimerManager().SetTimer(DecalTimerHandle,[this]()
	{
		if(ChargeDecalComponent)
		{
			ChargeDecalComponent->DestroyComponent();
			ChargeDecalComponent = nullptr;
		}
	},1.0f,false);

	// AI 경로 추적 중지 
	if(AAIController* AIController = Cast<AAIController>(GetController()))
	{
		if(UPathFollowingComponent* PathComp = AIController->GetPathFollowingComponent())
		{
			// 경로 추적 중단
			PathComp->Deactivate();

			AIController->StopMovement();
			UE_LOG(LogTemp,Warning,TEXT("AI movement forcibly stopped before LaunchCharacter"));
		}
	}

	if(SpecialSkillMontage)
	{
		if(UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
		{
			// 스페셜 스킬 몽타주에 플레이어에게 돌진하기 전, 준비 애니메이션(1초) 포함
			AnimInst->Montage_Play(SpecialSkillMontage);
			UE_LOG(LogTemp,Warning,TEXT("PerformSpecialSkill_Charge: Montage played"));

			// 경로 시각화 (기획 확인용)
			//DrawDebugLine(GetWorld(), ChargeStartLocation, ChargeTargetLocation, FColor::Red, false, 2.0f, 0, 3.0f);

			// 몽타주 종료 델리게이트 바인딩 (몽타주 종료 = 스킬 종료)
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this,&AEntityPreset::OnSkillMontageEnded);
			AnimInst->Montage_SetEndDelegate(EndDelegate,SpecialSkillMontage);

			// 준비 시간 후 돌진 실행
			float PrepTime = 1.0f;
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle,this,&AEntityPreset::StartChargeMovement,PrepTime,false);
		} else
		{
			UE_LOG(LogTemp,Error,TEXT("PerformSpecialSkill_Charge: AnimInstance not found"));
		}
	} else
	{
		UE_LOG(LogTemp,Error,TEXT("PerformSpecialSkill_Charge: SpecialSkillMontage is not set"));
	}
}

void AEntityPreset::ExecuteChargeDash()
{
	float LaunchSpeed = 2000.0f;

	// 실제 돌진
	LaunchCharacter(ChargeDirection * LaunchSpeed,true,true);
	bIsCharging = true;

	UE_LOG(LogTemp,Warning,TEXT("ExecuteChargeDash: Launch Started"));
}

void AEntityPreset::SpawnChargeIndicator(FVector Start,FVector End)
{
	if(!ChargeDecalComponent)
	{
		ChargeDecalComponent = NewObject<UDecalComponent>(this);
		ChargeDecalComponent->RegisterComponent();
		ChargeDecalComponent->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepWorldTransform);
		ChargeDecalComponent->SetDecalMaterial(ChargeDecalMaterial);
	}

	FVector MidPoint = (Start + End) * 0.5f;
	float Length = FVector::Distance(Start,End);

	// 데칼 크기: Y축 방향으로 돌진 경로 길이 반영
	ChargeDecalComponent->DecalSize = FVector(100.f,Length * 0.5f,100.f);  // (높이, Y폭, 깊이)

	// 데칼 방향: 돌진 방향으로 정렬 (Pitch -90은 지면 투사)
	// Z축 투사, Y축 방향 보정 (Yaw + 90도)
	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(Start,End);
	FRotator DecalRot = FRotator(-90.f,LookAtRot.Yaw + 90.f,0.f);

	ChargeDecalComponent->SetWorldLocation(MidPoint);
	ChargeDecalComponent->SetWorldRotation(DecalRot);
}


void AEntityPreset::StartChargeMovement()
{
	if(ChargeTimeline)
	{
		ChargeTimeline->PlayFromStart();
	}
}

void AEntityPreset::Timeline_ChargeProgress(float Value)
{
	FVector NewLocation = FMath::Lerp(ChargeStartLocation,ChargeTargetLocation,Value);
	SetActorLocation(NewLocation);
}

void AEntityPreset::Timeline_ChargeFinished()
{
	bIsCastingSkill = false;
	HideSpecialHitBox();
}

void AEntityPreset::ClearCastingSkill()
{
	bIsCastingSkill = false;
}

//  스킬 몽타주 종료 콜백
void AEntityPreset::OnSkillMontageEnded(UAnimMontage* Montage,bool bInterrupted)
{
	// 스킬 종료 처리 
	bIsCastingSkill = false;

	if(bIsBreaking)
	{
		bIsBreaking = false;
	}/*
	else if(bIsCharging)
	{
		bIsCharging = false;
	}
	else if(bIsFreezing)
	{
		bIsFreezing = false;
	}*/

	// AI 경로 추적 활성화
	if(AAIController* AIController = Cast<AAIController>(GetController()))
	{
		if(UBlackboardComponent* BBComp = AIController->GetBlackboardComponent())
		{
			BBComp->SetValueAsBool(BBKEY_BASKILLCONDITION,false);
			BBComp->SetValueAsBool(BBKEY_BBSKILLCONDITION,false);
			BBComp->ClearValue(BBKEY_CASTSKILLID); // string이라면 clear
		}

		if(UPathFollowingComponent* PathComp = AIController->GetPathFollowingComponent())
		{
			APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(),0);

			// 경로 추적 활성화 
			PathComp->Activate();
			AIController->SetFocus(PlayerPawn);

		}
	}
}

//  가드 종료 콜백
void AEntityPreset::OnGuardEnded()
{
	// 스킬 종료 처리 
	bIsCastingSkill = false;
	this->bIsDefending = false;
	UE_LOG(LogTemp,Warning,TEXT("방어 종료"));

	// AI 경로 추적 활성화
	if(AAIController* AIController = Cast<AAIController>(GetController()))
	{
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(),0);
		if(UPathFollowingComponent* PathComp = AIController->GetPathFollowingComponent())
		{
			// 경로 추적 활성화 
			PathComp->Activate();
			AIController->SetFocus(PlayerPawn);
		}

		// AI 로직 재시작
		if(AIController->BrainComponent)
		{
			AIController->BrainComponent->RestartLogic();
			UE_LOG(LogTemp,Warning,TEXT("OnGuardEnded → AI BrainComponent Restarted"));
		}
	}
	UE_LOG(LogTemp,Error,TEXT("OnGuardEnded"));

}

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

	//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 1.5f, 0, 5.f);
	UE_LOG(LogTemp,Warning,TEXT("DrawChargePath: Charge path drawn"));
}

void AEntityPreset::ApplyKnockbackEffect(ACharacter* Target,float Distance,float Duration)
{
	// LaunchCharacter 로 구현
	FVector KnockbackDir = Target->GetActorLocation() - GetActorLocation();
	KnockbackDir.Normalize();

	// 최소 보정
	if(Distance <= 0.01f)
	{
		Duration = 0.1f;
	}

	// 속도 = 거리/시간
	float KnockbackSpeed = Distance / Duration;

	FVector KnockbackVelocity = KnockbackDir * KnockbackSpeed;

	UCharacterMovementComponent* MoveComp = Target->GetCharacterMovement();
	if(MoveComp)
	{
		// 기존 모멘텀 무시하고 새로운 속도로 밀기
		Target->LaunchCharacter(KnockbackVelocity,true,true);

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
		Target->GetWorldTimerManager().SetTimer(FrictionRestoreHandle,RestoreFriction,Duration,false);

		UE_LOG(LogTemp,Warning,TEXT("KnockBack to %s → Distance: %.1fcm in %.2fs (speed: %.1f)"),*Target->GetName(),Distance,Duration,KnockbackSpeed);
	}
}

void AEntityPreset::SpawnProjectile_ThrowRock()
{
	// 스폰할 투사체 클래스 설정 확인
	if(!NormalProjectileClass)
	{
		UE_LOG(LogTemp,Error,TEXT("ProjectileClass not set!"));
		return;
	}
	
	// 스폰 위치, 방향 설정 
	FVector SpawnLocation = GetMesh()->GetSocketLocation(TEXT("ThrowRockSocket"));
	FVector Direction = GetMesh()->GetRightVector(); // 메시가 270도 회전된 상태가 X축 전방이기 때문에 Right Vector를 가져옴 
	//FVector Direction = GetMesh()->GetForwardVector();

	//FRotator SpawnRotation = GetMesh()->GetSocketRotation(TEXT("ThrowRockSocket"));

	// 스폰 파라미터
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	// 투사체 액터 스폰 
	FRotator DumyRotation = Direction.Rotation();
	AEntityProjectile* SpawnedProjectile = GetWorld()->SpawnActor<AEntityProjectile>(NormalProjectileClass,SpawnLocation,DumyRotation,SpawnParams);
	//AActor* SpawnedProjectile = GetWorld()->SpawnActor<AActor>(NormalProjectileClass,SpawnLocation,SpawnRotation,SpawnParams);

	if(SpawnedProjectile)
	{
		// Skill 데이터 테이블에서 "Skill_ThrowRock" 데이터 가져오기
		FSkillData SkillData;
		TArray<FSkillEffectData> EffectDataArray;

		if(UABGameSingleton::Get().GetSkillDataBySkillID("Skill_ThrowRock",SkillData) &&
			UABGameSingleton::Get().GetSkillEffectDataBySkillID("Skill_ThrowRock",EffectDataArray))
		{
			// 초기화
			SpawnedProjectile->InitProjectileBySkillData(SkillData,EffectDataArray);

			//SpawnedProjectile->IgnoreActorWhenMoving(this,true);

			UE_LOG(LogTemp,Error,TEXT("Spawned ThrowRock Projectile"));

			// 발사
			//FVector Direction = GetMesh()->GetForwardVector();
			SpawnedProjectile->FireInDirection(Direction);
		} else
		{
			UE_LOG(LogTemp,Error,TEXT("Failed to load Skill_ThrowRock data!"));
		}
	}
}
void AEntityPreset::SpawnProjectile_FireBall()
{
	// 스폰할 투사체 클래스 설정 확인
	if(!SpecialProjectileClass)
	{
		UE_LOG(LogTemp,Error,TEXT("ProjectileClass not set!"));
		return;
	}

	// 스폰 위치, 방향 설정 
	FVector SpawnLocation = GetMesh()->GetSocketLocation(TEXT("FireBallSocket"));
	FVector Direction = GetMesh()->GetRightVector(); // 메시가 270도 회전된 상태가 X축 전방이기 때문에 Right Vector를 가져옴 

	// 스폰 파라미터
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	// 투사체 액터 스폰 
	FRotator DumyRotation = Direction.Rotation();
	AEntityProjectile* SpawnedProjectile = GetWorld()->SpawnActor<AEntityProjectile>(SpecialProjectileClass,SpawnLocation,DumyRotation,SpawnParams);

	if(SpawnedProjectile)
	{
		// Skill 데이터 테이블에서 "Skill_FireBall" 데이터 가져오기
		FSkillData SkillData;
		TArray<FSkillEffectData> EffectDataArray;

		if(UABGameSingleton::Get().GetSkillDataBySkillID("Skill_FireBall",SkillData) &&
			UABGameSingleton::Get().GetSkillEffectDataBySkillID("Skill_FireBall",EffectDataArray))
		{
			// 초기화
			SpawnedProjectile->InitProjectileBySkillData(SkillData,EffectDataArray);

			UE_LOG(LogTemp,Error,TEXT("Spawned FireBall Projectile"));

			// 발사
			SpawnedProjectile->FireInDirection(Direction);
		} else
		{
			UE_LOG(LogTemp,Error,TEXT("Failed to load Skill_FireBall data!"));
		}
	}
}
void AEntityPreset::PerformSkill_FireBall()
{
	if(!SpecialSkillMontage) return;

	// 스킬 시전 플래그 설정
	// 해당 변수가 true일 동안엔 다른 스킬 시전 불가능
	bIsCastingSkill = true;

	//// AI 경로 추적 중지 
	//if(AAIController* AIController = Cast<AAIController>(GetController()))
	//{
	//	if(UPathFollowingComponent* PathComp = AIController->GetPathFollowingComponent())
	//	{
	//		// 경로 추적 중단
	//		PathComp->Deactivate();

	//		AIController->StopMovement();
	//		UE_LOG(LogTemp,Warning,TEXT("AI movement forcibly stopped before Launch Projectile"));
	//	}
	//}

	if(SpecialSkillMontage)
	{
		if(UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
		{
			// 스페셜 스킬 몽타주
			AnimInst->Montage_Play(SpecialSkillMontage);

			// 몽타주 종료 델리게이트 바인딩 (몽타주 종료 = 스킬 종료)
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this,&AEntityPreset::OnSkillMontageEnded);
			AnimInst->Montage_SetEndDelegate(EndDelegate,SpecialSkillMontage);
		} else
		{
			UE_LOG(LogTemp,Error,TEXT("PerformSpecialSkill_FireBall: AnimInstance not found"));
		}
	} else
	{
		UE_LOG(LogTemp,Error,TEXT("PerformSpecialSkill_FireBall: SpecialSkillMontage is not set"));
	}
}
void AEntityPreset::PerformSkill_FreezeBreath()
{
	if(!SpecialSkillMontage) return;

	// 스킬 시전 플래그 설정
	// 해당 변수가 true일 동안엔 다른 스킬 시전 불가능
	bIsCastingSkill = true;
	bIsFreezing = true;

	// AI 경로 추적 중지 
	if(AAIController* AIController = Cast<AAIController>(GetController()))
	{
		if(UPathFollowingComponent* PathComp = AIController->GetPathFollowingComponent())
		{
			// 경로 추적 중단
			PathComp->Deactivate();

			AIController->StopMovement();
			UE_LOG(LogTemp,Warning,TEXT("AI movement forcibly stopped before Breath"));
		}
	}

	if(SpecialSkillMontage)
	{
		if(UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
		{
			// 스페셜 스킬 몽타주
			AnimInst->Montage_Play(SpecialSkillMontage);

			// 몽타주 종료 델리게이트 바인딩 (몽타주 종료 = 스킬 종료)
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this,&AEntityPreset::OnSkillMontageEnded);
			AnimInst->Montage_SetEndDelegate(EndDelegate,SpecialSkillMontage);
		} else
		{
			UE_LOG(LogTemp,Error,TEXT("PerformSpecialSkill_FreezeBreath: AnimInstance not found"));
		}
	} else
	{
		UE_LOG(LogTemp,Error,TEXT("PerformSpecialSkill_FreezeBreath: SpecialSkillMontage is not set"));
	}
}

void AEntityPreset::PerformSkill_EarthBreaker()
{
	if(!SpecialSkillMontage) return;

	// 스킬 시전 플래그 설정
	// 해당 변수가 true일 동안엔 다른 스킬 시전 불가능
	bIsCastingSkill = true;
	bIsBreaking = true;

	// AI 경로 추적 중지 
	if(AAIController* AIController = Cast<AAIController>(GetController()))
	{
		if(UPathFollowingComponent* PathComp = AIController->GetPathFollowingComponent())
		{
			// 경로 추적 중단
			PathComp->Deactivate();

			AIController->StopMovement();
			UE_LOG(LogTemp,Warning,TEXT("AI movement forcibly stopped before Earth Breaking"));
		}
	}

	if(SpecialSkillMontage)
	{
		if(UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
		{
			// 스페셜 스킬 몽타주
			AnimInst->Montage_Play(SpecialSkillMontage);

			// 몽타주 종료 델리게이트 바인딩 (몽타주 종료 = 스킬 종료)
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this,&AEntityPreset::OnSkillMontageEnded);
			AnimInst->Montage_SetEndDelegate(EndDelegate,SpecialSkillMontage);
			UE_LOG(LogTemp,Error,TEXT("PerformSkill_EarthBreaker"));
		} else
		{
			UE_LOG(LogTemp,Error,TEXT("PerformSkill_EarthBreaker: AnimInstance not found"));
		}
	} else
	{
		UE_LOG(LogTemp,Error,TEXT("PerformSkill_EarthBreaker: SpecialSkillMontage is not set"));
	}
}

void AEntityPreset::PerformSkill_Arrow()
{
	if(!NormalProjectileClass) return;

	// 스킬 시전 플래그 설정
	// 해당 변수가 true일 동안엔 다른 스킬 시전 불가능
	bIsCastingSkill = true;

	// AI 경로 추적 중지 
	if(AAIController* AIController = Cast<AAIController>(GetController()))
	{
		if(UPathFollowingComponent* PathComp = AIController->GetPathFollowingComponent())
		{
			// 경로 추적 중단
			PathComp->Deactivate();
			// 포커스 해제 
			AIController->ClearFocus(EAIFocusPriority::Gameplay);

			AIController->StopMovement();
			UE_LOG(LogTemp,Warning,TEXT("AI movement forcibly stopped before Spawn Projectile"));
		}
	}

	if(NormalProjectileClass)
	{
		if(UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
		{
			// 스페셜 스킬 몽타주
			AnimInst->Montage_Play(NormalSkillMontage);

			// 몽타주 종료 델리게이트 바인딩 (몽타주 종료 = 스킬 종료)
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this,&AEntityPreset::OnSkillMontageEnded);
			AnimInst->Montage_SetEndDelegate(EndDelegate,NormalSkillMontage);
			bUseControllerRotationYaw = true;
		} else
		{
			UE_LOG(LogTemp,Error,TEXT("PerformSkill_Arrow: AnimInstance not found"));
		}
	} else
	{
		UE_LOG(LogTemp,Error,TEXT("PerformSkill_Arrow: NormalSkillMontage is not set"));
	}
}

void AEntityPreset::PerformSkill_SplinterArrow()
{
	if(!SpecialProjectileClass) return;

	// 스킬 시전 플래그 설정
	// 해당 변수가 true일 동안엔 다른 스킬 시전 불가능
	bIsCastingSkill = true;

	// AI 경로 추적 중지 
	if(AAIController* AIController = Cast<AAIController>(GetController()))
	{
		if(UPathFollowingComponent* PathComp = AIController->GetPathFollowingComponent())
		{
			// 경로 추적 중단
			PathComp->Deactivate();
			// 포커스 해제 
			AIController->ClearFocus(EAIFocusPriority::Gameplay);

			AIController->StopMovement();
			UE_LOG(LogTemp,Warning,TEXT("AI movement forcibly stopped before Spawn Splineter Projectile"));
		}
	}

	if(SpecialProjectileClass)
	{
		if(UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
		{
			// 스페셜 스킬 몽타주
			AnimInst->Montage_Play(SpecialSkillMontage);

			UE_LOG(LogTemp,Error,TEXT("PerformSkill_SplinterArrow: Anim Montage Play"));

			// 몽타주 종료 델리게이트 바인딩 (몽타주 종료 = 스킬 종료)
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this,&AEntityPreset::OnSkillMontageEnded);
			AnimInst->Montage_SetEndDelegate(EndDelegate,SpecialSkillMontage);
		} else
		{
			UE_LOG(LogTemp,Error,TEXT("PerformSkill_SplinterArrow: AnimInstance not found"));
		}
	} else
	{
		UE_LOG(LogTemp,Error,TEXT("PerformSkill_SplinterArrow: SpecialSkillMontage is not set"));
	}
}

// 노티파이에서 실행 : 화살 발사
void AEntityPreset::FireProjectile_Arrow()
{
	// 스폰할 투사체 클래스 설정 확인
	if(!NormalProjectileClass)
	{
		UE_LOG(LogTemp,Error,TEXT("ProjectileClass not set!"));
		return;
	}

	// 스폰 위치, 방향 설정 
	FVector SpawnLocation = GetMesh()->GetSocketLocation(TEXT("ArrowSocket"));
	FVector Direction = GetMesh()->GetRightVector(); // 메시가 270도 회전된 상태가 X축 전방이기 때문에 Right Vector를 가져옴 

	// 스폰 파라미터
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	// 투사체 액터 스폰 
	FRotator DumyRotation = Direction.Rotation();
	AEntityProjectile* SpawnedProjectile = GetWorld()->SpawnActor<AEntityProjectile>(NormalProjectileClass,SpawnLocation,DumyRotation,SpawnParams);

	if(SpawnedProjectile)
	{
		// Skill 데이터 테이블에서 "Skill_Arrow" 데이터 가져오기
		FSkillData SkillData;
		TArray<FSkillEffectData> EffectDataArray;

		if(UABGameSingleton::Get().GetSkillDataBySkillID("Skill_Arrow",SkillData) &&
			UABGameSingleton::Get().GetSkillEffectDataBySkillID("Skill_Arrow",EffectDataArray))
		{
			// 초기화
			SpawnedProjectile->InitProjectileBySkillData(SkillData,EffectDataArray);

			//SpawnedProjectile->IgnoreActorWhenMoving(this,true);

			UE_LOG(LogTemp,Error,TEXT("Spawned Skill_Arrow Projectile"));

			// 발사
			//FVector Direction = GetMesh()->GetForwardVector();
			SpawnedProjectile->FireInDirection(Direction);
			bUseControllerRotationYaw = true;
		} else
		{
			UE_LOG(LogTemp,Error,TEXT("Failed to load Skill_Arrow data!"));
		}
	}
}

// 노티파이에서 실행 : 센터 화살 스폰 
//void AEntityPreset::Spawn_CenterArrow()
//{
//	// 스폰할 투사체 클래스 설정 확인
//	if(!SpecialProjectileClass)
//	{
//		UE_LOG(LogTemp,Error,TEXT("ProjectileClass not set!"));
//		return;
//	}
//
//	// 스폰 위치, 방향 설정 
//	FVector SpawnLocation = GetMesh()->GetSocketLocation(TEXT("ArrowSocket"));
//	FVector Direction = GetMesh()->GetRightVector(); // 메시가 270도 회전된 상태가 X축 전방이기 때문에 Right Vector를 가져옴 
//
//	// 스폰 파라미터
//	FActorSpawnParameters SpawnParams;
//	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
//	SpawnParams.Owner = this;
//	SpawnParams.Instigator = GetInstigator();
//
//	// 투사체 액터 스폰 
//	//FRotator DumyRotation = Direction.Rotation();
//	FRotator DumyRotation = GetActorForwardVector().Rotation();
//	Arrow = GetWorld()->SpawnActor<AEntityProjectile>(SpecialProjectileClass,SpawnLocation,DumyRotation,SpawnParams);
//
//	if(Arrow)
//	{
//		// Skill 데이터 테이블에서 "Skill_SplinterArrow" 데이터 가져오기
//		FSkillData SkillData;
//		TArray<FSkillEffectData> EffectDataArray;
//
//		if(UABGameSingleton::Get().GetSkillDataBySkillID("Skill_SplinterArrow",SkillData) &&
//			UABGameSingleton::Get().GetSkillEffectDataBySkillID("Skill_SplinterArrow",EffectDataArray))
//		{
//			// 초기화
//			Arrow->InitProjectileBySkillData(SkillData,EffectDataArray);
//
//			UE_LOG(LogTemp,Error,TEXT("Spawned Skill_SplinterArrow Center Projectile"));
//
//			// 발사
//			/*FTransform SocketTransform = GetMesh()->GetSocketTransform(TEXT("ArrowSocket"),RTS_World);
//			FVector FireDirection = SocketTransform.GetRotation().GetForwardVector();
//
//			Arrow->FireInDirection(FireDirection);
//			UE_LOG(LogTemp,Warning,TEXT("Fired arrow → Dir: %s"), *FireDirection.ToString());*/
//
//			/*FVector Direction = GetMesh()->GetForwardVector();
//			Arrow->FireInDirection(Direction);*/
//		} else
//		{
//			UE_LOG(LogTemp,Error,TEXT("Failed to load Skill_SplinterArrow Center data!"));
//		}
//	}
//}

// 노티파이에서 실행 : 화살 스폰 및 모든 화살 발사  
void AEntityPreset::Fire_AllArrows()
{
	if(!SpecialProjectileClass)
	{
		UE_LOG(LogTemp,Error,TEXT("ProjectileClass not set!"));
		return;
	}

	TArray<AEntityProjectile*> ArrowList;

	// Skill 데이터 불러오기 (한 번만)
	FSkillData SkillData;
	TArray<FSkillEffectData> EffectDataArray;

	//// 센터 화살 포함
	//if(Arrow)
	//{
	//	//ArrowList.Add(Arrow);
	//}

	TArray<FName> Sockets = {TEXT("ArrowSocket"), TEXT("ArrowSubSocket_1"),TEXT("ArrowSubSocket_2")};

	// 서브 화살 생성
	for(FName SocketName : Sockets)
	{
		FVector SpawnLoc = GetMesh()->GetSocketLocation(SocketName);
		FRotator SpawnRot = GetMesh()->GetSocketRotation(SocketName);
		//FVector Direction = SpawnRot.Vector();

		// 스폰 파라미터
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		//AEntityProjectile*
		Arrow = GetWorld()->SpawnActor<AEntityProjectile>(SpecialProjectileClass,SpawnLoc,SpawnRot,SpawnParams);

		if(Arrow)
		{
			if(UABGameSingleton::Get().GetSkillDataBySkillID("Skill_SplinterArrow",SkillData) &&
			UABGameSingleton::Get().GetSkillEffectDataBySkillID("Skill_SplinterArrow",EffectDataArray))
			{
				// 초기화
				Arrow->InitProjectileBySkillData(SkillData,EffectDataArray);

				//UE_LOG(LogTemp,Error,TEXT("Failed to load Skill_SplinterArrow sub data!"));

				ArrowList.Add(Arrow);
			}
		}
	}

	// 모든 화살 발사
	for(int i = 0; i < ArrowList.Num(); i++)
	{
		AEntityProjectile* CurrentArrow = ArrowList[i];
		FName SocketName;

		/*if(i == 0)
			SocketName = TEXT("ArrowSubSocket_1");
		else if(i == 1)
			SocketName = TEXT("ArrowSubSocket_2");
		*/	
		if(i == 0)
			SocketName = TEXT("ArrowSocket");
		else if(i == 1)
			SocketName = TEXT("ArrowSubSocket_1");
		else if(i == 2)
			SocketName = TEXT("ArrowSubSocket_2");

		if(CurrentArrow)
		{
			/*FVector SpawnLocation = GetMesh()->GetSocketLocation(SocketName);
			FVector FireDirection = GetMesh()->GetSocketRotation(SocketName).Vector();*/
			/*FVector FireDirection = GetMesh()->GetSocketRotation(SocketName).Vector();
			CurrentArrow->FireInDirection(FireDirection);*/

			FTransform SocketTransform = GetMesh()->GetSocketTransform(SocketName,RTS_World);
			FVector FireDirection = SocketTransform.GetRotation().GetForwardVector();

			CurrentArrow->FireInDirection(FireDirection);
			bUseControllerRotationYaw = true;
			UE_LOG(LogTemp,Warning,TEXT("Fired arrow %d → Dir: %s"),i,*FireDirection.ToString());
		}
	}

	//for(int32 i = 0; i < ArrowList.Num(); ++i)
	//{
	//	AEntityProjectile* CurrentArrow = ArrowList[i];
	//	if(CurrentArrow)
	//	{
	//		// 소켓 회전 기준 방향
	//		FVector FireDir = CurrentArrow->GetActorForwardVector();
	//		//CurrentArrow->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	//		CurrentArrow->bAutoFireOnSpawn = true;
	//		CurrentArrow->FireInDirection(FireDir);

	//		UE_LOG(LogTemp,Warning,TEXT("Fired arrow %d → Dir: %s"),i,*FireDir.ToString());
	//	}
	//}

	//// 센터 화살 포인터 정리
	//Arrow = nullptr;
}

//void AEntityPreset::Fire_AllArrows()
//{
//	// 스폰할 투사체 클래스 설정 확인
//	if(!SpecialProjectileClass)
//	{
//		UE_LOG(LogTemp,Error,TEXT("ProjectileClass not set!"));
//		return;
//	}
//
//	// 모든 화살 발사를 위해 저장할 배열
//	TArray<AEntityProjectile*> ArrowList;
//
//	if(Arrow)
//	{
//		ArrowList.Add(Arrow);
//	}
//
//	TArray<FName> SubSockets = {TEXT("ArrowSubSocket_1"),TEXT("ArrowSubSocket_2")};
//
//	// 서브 화살 스폰 
//	for(int i = 0; i < SubSockets.Num(); i++)
//	{
//		// 스폰 위치, 방향 설정 
//		FVector SpawnLocation = GetMesh()->GetSocketLocation(SubSockets[i]);
//		FVector Direction = GetMesh()->GetRightVector(); // 메시가 270도 회전된 상태가 X축 전방이기 때문에 Right Vector를 가져옴 
//		FRotator SpawnRotation = Direction.Rotation();
//
//		// 스폰 파라미터
//		FActorSpawnParameters SpawnParams;
//		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
//		SpawnParams.Owner = this;
//		SpawnParams.Instigator = GetInstigator();
//
//		AEntityProjectile* SubArrow = GetWorld()->SpawnActor<AEntityProjectile>(SpecialProjectileClass,SpawnLocation,SpawnRotation,SpawnParams);
//
//		if(SubArrow)
//		{
//			// Skill 데이터 테이블에서 "Skill_SplinterArrow" 데이터 가져오기
//			FSkillData SkillData;
//			TArray<FSkillEffectData> EffectDataArray;
//
//			if(UABGameSingleton::Get().GetSkillDataBySkillID("Skill_SplinterArrow",SkillData) &&
//				UABGameSingleton::Get().GetSkillEffectDataBySkillID("Skill_SplinterArrow",EffectDataArray))
//			{
//				// 초기화
//				Arrow->InitProjectileBySkillData(SkillData,EffectDataArray);
//
//				UE_LOG(LogTemp,Error,TEXT("Spawned Skill_SplinterArrow Projectile"));
//			} else
//			{
//				UE_LOG(LogTemp,Error,TEXT("Failed to load Skill_SplinterArrow data!"));
//			}
//		}
//		//// 투사체 액터 스폰 
//		//if(i == 0)
//		//{
//		//	FRotator DumyRotation = Direction.Rotation();
//		//	SubArrow1 = GetWorld()->SpawnActor<AEntityProjectile>(NormalProjectileClass,SpawnLocation,DumyRotation,SpawnParams);
//		//}
//		//else if(i == 1)
//		//{
//		//	FRotator DumyRotation = Direction.Rotation();
//		//	SubArrow2 = GetWorld()->SpawnActor<AEntityProjectile>(NormalProjectileClass,SpawnLocation,DumyRotation,SpawnParams);
//		//}
//	}
//
//	for(int i = 0; i < ArrowList.Num(); i++)
//	{
//		AEntityProjectile* CurrentArrow = ArrowList[i];
//
//		if(CurrentArrow)
//		{
//			FVector FireDirection = GetMesh()->GetForwardVector();
//			CurrentArrow->FireInDirection(FireDirection);
//		}
//	}
//
//}

void AEntityPreset::PerformSkill_ShieldGuard()
{
	if(!SpecialSkillMontage) return;

	UE_LOG(LogTemp,Error,TEXT("PerformSkill_ShieldGuard: 스킬 시전"));

	// 스킬 시전 플래그 설정
	// 해당 변수가 true일 동안엔 다른 스킬 시전 불가능
	bIsCastingSkill = true;
	this->bIsDefending = true;

	// AI 경로 추적 중지 
	if(AAIController* AIController = Cast<AAIController>(GetController()))
	{
		if(UPathFollowingComponent* PathComp = AIController->GetPathFollowingComponent())
		{
			// 경로 추적 중단
			PathComp->Deactivate();

			AIController->StopMovement();
			UE_LOG(LogTemp,Warning,TEXT("AI movement forcibly stopped before Shield"));
		}
	}

	if(UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
	{
		// 스페셜 스킬 몽타주
		AnimInst->Montage_Play(SpecialSkillMontage);

		UE_LOG(LogTemp,Error,TEXT("PerformSkill_ShieldGuard: Anim Montage Play"));

		// 스킬 시전 3초 후 스킬 종료 
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle,this,&AEntityPreset::OnGuardEnded,3.0f,false);
	} else
	{
		UE_LOG(LogTemp,Error,TEXT("PerformSkill_ShieldGuard: AnimInstance not found"));
	}
}


EnumAttackType AEntityPreset::GetAttackType()
{
	UE_LOG(LogTemp,Warning,TEXT("Get AttackType: %d"),(uint8)currentAttackType);

	return currentAttackType;
}

float AEntityPreset::GetNormalSkillRange()
{
	UE_LOG(LogTemp,Warning,TEXT("Get Normal Skill Range: %f"),currentNormalSkillRange);

	return currentNormalSkillRange;
}

float AEntityPreset::GetSpecialSkillRange()
{
	UE_LOG(LogTemp,Warning,TEXT("Get Special Skill Range: %f"),currentSpecialSkillRange);

	return currentSpecialSkillRange;
}


// 화살 부착 시도했던 로직
// 혹시 몰라 남겨줌 
//void AEntityPreset::SpawnProjectile_Arrow()
//{
//	if(Arrow)
//	{
//		Arrow ->Destroy();
//		Arrow = nullptr;
//	}
//
//	// 스폰 파라미터
//	FActorSpawnParameters SpawnParams;
//	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
//	SpawnParams.Owner = this;
//
//	FVector SpawnLocation = GetMesh()->GetSocketLocation(TEXT("ArrowSocket"));
//	FRotator SpawnRotation = GetActorForwardVector().Rotation();
//	
//	//// pitch, yaw, roll: x, z, y
//	//FRotator NewRotation = FRotator(0.f,0.f,-90.f);
//	//// 화살 전방 각도 보정 
//	//FRotator FixedRotation = SpawnRotation + NewRotation;
//
//	Arrow = GetWorld()->SpawnActor<AEntityProjectile>(NormalProjectileClass,SpawnLocation,SpawnRotation,SpawnParams);
//
//	if(!Arrow)
//	{
//		UE_LOG(LogTemp,Error,TEXT("Failed to spawn arrow"));
//	}
//
//	Arrow->AttachToComponent(GetMesh(),FAttachmentTransformRules::SnapToTargetNotIncludingScale,TEXT("ArrowSocket"));
//	//Arrow->SetActorHiddenInGame(false);
//	//Arrow->SetActorEnableCollision(false);
//
//	// Skill 데이터 테이블에서 "Skill_Arrow" 데이터 가져오기
//	FSkillData SkillData;
//	TArray<FSkillEffectData> EffectDataArray;
//	if(UABGameSingleton::Get().GetSkillDataBySkillID("Skill_Arrow",SkillData) &&
//		UABGameSingleton::Get().GetSkillEffectDataBySkillID("Skill_Arrow",EffectDataArray))
//	{
//		Arrow->InitProjectileBySkillData(SkillData,EffectDataArray);
//	}
//	else
//	{
//		UE_LOG(LogTemp,Error,TEXT("Failed to load Skill_Arrow data!"));
//	}
//}
//
//
//void AEntityPreset::SpawnProjectile_Arrow()
//{
//	//FActorSpawnParameters SpawnParams;
//	//SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
//	//SpawnParams.Owner = this;
//	//SpawnParams.Instigator = GetInstigator();
//
//	//Arrow = GetWorld()->SpawnActor<AEntityProjectile>(
//	//	NormalProjectileClass,
//	//	FVector::ZeroVector,
//	//	FRotator::ZeroRotator,
//	//	SpawnParams);
//
//	//Arrow->AttachToComponent(
//	//	GetMesh(),
//	//	FAttachmentTransformRules::SnapToTargetNotIncludingScale,
//	//	TEXT("ArrowSocket"));
//
//
//	//FTransform SocketTransform = GetMesh()->GetSocketTransform(TEXT("ArrowSocket"),RTS_World);
//	//Arrow->SetActorTransform(SocketTransform);
//	//Arrow->SetActorRelativeRotation(FRotator(0,0,270)); // 필요 시 보정
//	//ArrowDirection = GetMesh()->GetSocketRotation(TEXT("ArrowSocket")).Vector();
//
//	//FSkillData SkillData;
//	//TArray<FSkillEffectData> EffectDataArray;
//
//	//if(UABGameSingleton::Get().GetSkillDataBySkillID("Skill_Arrow",SkillData) &&
//	//	UABGameSingleton::Get().GetSkillEffectDataBySkillID("Skill_Arrow",EffectDataArray))
//	//{
//	//	// 초기화
//	//	Arrow->InitProjectileBySkillData(SkillData,EffectDataArray);
//	//	UE_LOG(LogTemp,Error,TEXT("Spawned Arrow Projectile"));
//	//} else
//	//{
//	//	UE_LOG(LogTemp,Error,TEXT("Failed to load Skill_Arrow data!"));
//	//}
// //
//	////FRotator FixedRotation = SocketTransform.GetRotation().Rotator() + FRotator(0.f,90.f,0.f);
//	////SpawnedArrow->SetActorRotation(FixedRotation);
//
//	////if(Arrow)
//	////{
//	////	//SkillArrowChildComponent->SetVisibility(true);
//	////	//SkillArrowChildComponent->SetHiddenInGame(false);
//	////	///*Arrow->SetActorEnableCollision(true);
//	////	//Arrow->SetActorHiddenInGame(false);*/
//
//	////	//// 화살 스폰 후 즉시 방향 확인
//	////	//UE_LOG(LogTemp,Warning,TEXT("Arrow Dir: %s"),*Arrow->GetActorForwardVector().ToString());
//	////	//UE_LOG(LogTemp,Warning,TEXT("Expected Dir: %s"),*GetActorForwardVector().ToString());
//
//	////	////Arrow->SetActorRelativeRotation(FRotator(0,0,270)); // 필요 시 보정
//
//	////	//FSkillData SkillData;
//	////	//TArray<FSkillEffectData> EffectDataArray;
//
//	////	//if(UABGameSingleton::Get().GetSkillDataBySkillID("Skill_Arrow",SkillData) &&
//	////	//	UABGameSingleton::Get().GetSkillEffectDataBySkillID("Skill_Arrow",EffectDataArray))
//	////	//{
//	////	//	// 초기화
//	////	//	Arrow->InitProjectileBySkillData(SkillData,EffectDataArray);
//	////	//	UE_LOG(LogTemp,Error,TEXT("Spawned Arrow Projectile"));
//	////	//} else
//	////	//{
//	////	//	UE_LOG(LogTemp,Error,TEXT("Failed to load Skill_Arrow data!"));
//	////	//}
//	////}
//}
//
//// 노티파이에서 실행 : 화살 발사
//void AEntityPreset::FireProjectile_Arrow()
//{
//	// 스폰할 투사체 클래스 설정 확인
//	if(!NormalProjectileClass)
//	{
//		UE_LOG(LogTemp,Error,TEXT("ProjectileClass not set!"));
//		return;
//	}
//
//	// 스폰 위치, 방향 설정 
//	FVector SpawnLocation = GetMesh()->GetSocketLocation(TEXT("ArrowSocket"));
//	FVector Direction = GetMesh()->GetRightVector(); // 메시가 270도 회전된 상태가 X축 전방이기 때문에 Right Vector를 가져옴 
//
//	// 스폰 파라미터
//	FActorSpawnParameters SpawnParams;
//	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
//	SpawnParams.Owner = this;
//	SpawnParams.Instigator = GetInstigator();
//
//	// 투사체 액터 스폰 
//	FRotator DumyRotation = Direction.Rotation();
//	AEntityProjectile* SpawnedProjectile = GetWorld()->SpawnActor<AEntityProjectile>(NormalProjectileClass,SpawnLocation,DumyRotation,SpawnParams);
//
//	if(SpawnedProjectile)
//	{
//		// Skill 데이터 테이블에서 "Skill_Arrow" 데이터 가져오기
//		FSkillData SkillData;
//		TArray<FSkillEffectData> EffectDataArray;
//
//		if(UABGameSingleton::Get().GetSkillDataBySkillID("Skill_Arrow",SkillData) &&
//			UABGameSingleton::Get().GetSkillEffectDataBySkillID("Skill_Arrow",EffectDataArray))
//		{
//			// 초기화
//			SpawnedProjectile->InitProjectileBySkillData(SkillData,EffectDataArray);
//
//			//SpawnedProjectile->IgnoreActorWhenMoving(this,true);
//
//			UE_LOG(LogTemp,Error,TEXT("Spawned Skill_Arrow Projectile"));
//
//			// 발사
//			//FVector Direction = GetMesh()->GetForwardVector();
//			SpawnedProjectile->FireInDirection(Direction);
//		} else
//		{
//			UE_LOG(LogTemp,Error,TEXT("Failed to load Skill_Arrow data!"));
//		}
//	}
//
//	//FActorSpawnParameters SpawnParams;
//	//SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
//	//SpawnParams.Owner = this;
//	//SpawnParams.Instigator = GetInstigator();
//
//
//
//	////Arrow->AttachToComponent(
//	////	GetMesh(),
//	////	FAttachmentTransformRules::SnapToTargetNotIncludingScale,
//	////	TEXT("ArrowSocket"));
//
//
//	//FTransform SocketTransform = GetMesh()->GetSocketTransform(TEXT("ArrowSocket"),RTS_World);
//	////Arrow->SetActorTransform(SocketTransform);
//	////Arrow->SetActorRelativeRotation(FRotator(0,0,270)); // 필요 시 보정
//	//ArrowDirection = GetMesh()->GetSocketRotation(TEXT("ArrowSocket")).Vector();
//
//	//Arrow = GetWorld()->SpawnActor<AEntityProjectile>(
//	//NormalProjectileClass,
//	//SocketTransform.GetLocation(),
//	//SocketTransform.GetRotation(),
//	//SpawnParams);
//
//	//FSkillData SkillData;
//	//TArray<FSkillEffectData> EffectDataArray;
//
//	//if(UABGameSingleton::Get().GetSkillDataBySkillID("Skill_Arrow",SkillData) &&
//	//	UABGameSingleton::Get().GetSkillEffectDataBySkillID("Skill_Arrow",EffectDataArray))
//	//{
//	//	// 초기화
//	//	Arrow->InitProjectileBySkillData(SkillData,EffectDataArray);
//	//	UE_LOG(LogTemp,Error,TEXT("Spawned Arrow Projectile"));
//	//} else
//	//{
//	//	UE_LOG(LogTemp,Error,TEXT("Failed to load Skill_Arrow data!"));
//	//}
//
//
//
//
//	//if(Arrow)
//	//{
//	//	// 소켓에서 화살 분리 
//	//	Arrow->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
//
//	//	//ArrowDirection = GetMesh()->GetRightVector();
//
//	//	// 자동 발사 막기
//	//	Arrow->bAutoFireOnSpawn = true;
//	//	Arrow->FireInDirection(ArrowDirection);
//	//}
//}
////void AEntityPreset::SpawnProjectile_Arrow()
////{
////	Arrow = Cast<AEntityProjectile>(SkillArrowChildComponent->GetChildActor());
////
////	if(Arrow)
////	{
////		Arrow->SetActorHiddenInGame(false);
////		Arrow->SetActorEnableCollision(true);
////		//Arrow->AttachToComponent(GetMesh(),FAttachmentTransformRules::SnapToTargetNotIncludingScale,TEXT("ArrowSocket"));
////
////		//FRotator SocketRotation = GetMesh()->GetSocketRotation(TEXT("ArrowSocket"));
////
////		////// 소켓 회전에 Yaw 180도 추가
////		////FRotator FixedRotation = SocketRotation + FRotator(0.f,180.f,0.f);
////
////		////Arrow->SetActorRelativeRotation(FixedRotation);
////		//
////		//// 궁수의 전방 벡터 기준으로 회전값 생성
////		//FVector ForwardDir = GetActorForwardVector(); // 또는 필요 시 GetMesh()->GetForwardVector()
////		//FRotator ArrowRotation = ForwardDir.Rotation();
////
////		//// 보정이 필요하면 여기서 추가 보정 (ex. Roll이나 Pitch 조정)
////		//Arrow->SetActorRotation(ArrowRotation);
////
////		//// 화살 스폰 후 즉시 방향 확인
////		UE_LOG(LogTemp,Warning,TEXT("Arrow Dir: %s"),*Arrow->GetActorForwardVector().ToString());
////		UE_LOG(LogTemp,Warning,TEXT("Expected Dir: %s"),*GetActorForwardVector().ToString());
////
////		//Arrow->SetActorRelativeRotation(FRotator(0,0,270)); // 필요 시 보정
////
////		FSkillData SkillData;
////		TArray<FSkillEffectData> EffectDataArray;
////
////		if(UABGameSingleton::Get().GetSkillDataBySkillID("Skill_Arrow",SkillData) &&
////			UABGameSingleton::Get().GetSkillEffectDataBySkillID("Skill_Arrow",EffectDataArray))
////		{
////			// 초기화
////			Arrow->InitProjectileBySkillData(SkillData,EffectDataArray);
////			UE_LOG(LogTemp,Error,TEXT("Spawned Arrow Projectile"));
////		} else
////		{
////			UE_LOG(LogTemp,Error,TEXT("Failed to load Skill_Arrow data!"));
////		}
////	}
////
////	//// 스폰할 투사체 클래스 설정 확인
////	//if(!NormalProjectileClass)
////	//{
////	//	UE_LOG(LogTemp,Error,TEXT("ProjectileClass not set!"));
////	//	return;
////	//}
////
////	//ArrowDirection = GetMesh()->GetSocketRotation(TEXT("ArrowSocket")).Vector();
////	//
////	//// 스폰 위치, 방향 설정 
////	//FTransform SocketTransform = GetMesh()->GetSocketTransform(TEXT("ArrowSocket"));
////
////	//// 스폰 파라미터
////	//FActorSpawnParameters SpawnParams;
////	//SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
////	//SpawnParams.Owner = this;
////	//SpawnParams.Instigator = GetInstigator();
////
////	//// 화살 스폰 
////	//PendingArrow = GetWorld()->SpawnActor<AEntityProjectile>(NormalProjectileClass,FVector::ZeroVector,FRotator::ZeroRotator,SpawnParams);
////	//PendingArrow->AttachToComponent(GetMesh(),FAttachmentTransformRules::SnapToTargetNotIncludingScale,TEXT("ArrowSocket"));
////
////	//// 화살 스폰 위치 설정 
////	//PendingArrow->SetActorLocation(SocketTransform.GetLocation());
////	//// 어태치용 z 270도 회전 적용 
////	//PendingArrow->SetActorRotation(SocketTransform.GetRotation().Rotator() + FRotator(0.f,0.f,270.f));
////	////FRotator RotationOffset(0.f,0.f,270.f);
////	////PendingArrow->SetActorRelativeRotation(RotationOffset);
////
////	//UE_LOG(LogTemp,Warning,TEXT("Arrow attached to socket."));
////	//
////	////ArrowDirection = GetMesh()->GetSocketRotation(TEXT("ArrowSocket")).Vector();
////
////	// Skill 데이터 테이블에서 "Skill_Arrow" 데이터 가져오기
////	//FSkillData SkillData;
////	//TArray<FSkillEffectData> EffectDataArray;
////
////	//if(UABGameSingleton::Get().GetSkillDataBySkillID("Skill_Arrow",SkillData) &&
////	//	UABGameSingleton::Get().GetSkillEffectDataBySkillID("Skill_Arrow",EffectDataArray))
////	//{
////	//	// 초기화
////	//	PendingArrow->InitProjectileBySkillData(SkillData,EffectDataArray);
////	//	UE_LOG(LogTemp,Error,TEXT("Spawned Arrow Projectile"));
////	//} 
////	//else
////	//{
////	//	UE_LOG(LogTemp,Error,TEXT("Failed to load Skill_Arrow data!"));
////	//}
////}
////void AEntityPreset::SpawnProjectile_Arrow()
////{
////	// 스폰할 투사체 클래스 설정 확인
////	if(!NormalProjectileClass)
////	{
////		UE_LOG(LogTemp,Error,TEXT("ProjectileClass not set!"));
////		return;
////	}
////
////	//// 스폰 위치, 방향 설정 
////	//FTransform SocketTransform = GetMesh()->GetSocketTransform(TEXT("ArrowSocket"),RTS_World);
////	//FVector SpawnLocation = SocketTransform.GetLocation();
////	//FRotator SpawnRotation  = SocketTransform.GetRotation().Rotator();
////
////	//// 스폰 파라미터
////	//FActorSpawnParameters SpawnParams;
////	//SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
////	//SpawnParams.Owner = this;
////	//SpawnParams.Instigator = GetInstigator();
////
////	////ArrowDirection = GetActorForwardVector();
////	//ArrowDirection = GetMesh()->GetSocketRotation(TEXT("ArrowSocket")).Vector(); // 소켓 방향 그대로
////	//FRotator DummyRotation = ArrowDirection.Rotation();
////
////	//// 화살 스폰 
////	//PendingArrow = GetWorld()->SpawnActor<AEntityProjectile>(NormalProjectileClass,SpawnLocation,DummyRotation,SpawnParams);
////
////
////	//
////
////	////FRotator DumyRotation = FRotator::ZeroRotator;
////	////FRotator DumyRotation = ArrowDirection.Rotation();
////	////AEntityProjectile* SpawnedProjectile = GetWorld()->SpawnActor<AEntityProjectile>(NormalProjectileClass,SpawnLocation,DumyRotation,SpawnParams);
////	//
////
////	if(PendingArrow)
////	{
////		// 소켓 트랜스폼 가져오기 (World 기준)
////		//FTransform SocketTransform = GetMesh()->GetSocketTransform(TEXT("ArrowSocket"),RTS_World);
////
////		// 위치와 회전 모두 수동으로 설정
////		//PendingArrow->SetActorLocation(SocketTransform.GetLocation());
////		//PendingArrow->SetActorRotation(SocketTransform.GetRotation().Rotator());
////
////		// 부착은 Scale 영향 X
////		//PendingArrow->AttachToComponent(GetMesh(),FAttachmentTransformRules::KeepWorldTransform,TEXT("ArrowSocket"));
////
////		
////
////		//PendingArrow->AttachToComponent(GetMesh(),FAttachmentTransformRules::KeepWorldTransform,TEXT("ArrowSocket"));
////
////		// 소켓의 월드 회전 방향을 따라가도록 화살의 로컬 회전 보정
////		FRotator SocketRotation = GetMesh()->GetSocketRotation(TEXT("ArrowSocket"));
////		FRotator LocalRotation = GetMesh()->GetSocketTransform(TEXT("ArrowSocket"),RTS_ParentBoneSpace).Rotator();
////
////		PendingArrow->SetActorRelativeRotation(LocalRotation);
////
////		PendingArrow->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, TEXT("ArrowSocket"));
////
////		//UE_LOG(LogTemp,Warning,TEXT("Arrow attached to ArrowSocket: Loc=%s, Rot=%s"),
////		//	*SocketTransform.GetLocation().ToString(),
////		//	*SocketTransform.GetRotation().Rotator().ToString());
////		////PendingArrow->AttachToComponent(GetMesh(),FAttachmentTransformRules::SnapToTargetIncludingScale,TEXT("ArrowSocket"));
////		//PendingArrow->AttachToComponent(GetMesh(),FAttachmentTransformRules::KeepWorldTransform,TEXT("ArrowSocket"));
////
////		//FTransform SocketTransform = GetMesh()->GetSocketTransform(TEXT("ArrowSocket"),RTS_World);
////		//PendingArrow->SetActorLocationAndRotation(SocketTransform.GetLocation(),SocketTransform.GetRotation());
////
////		//UE_LOG(LogTemp,Warning,TEXT("Arrow attached to socket."));
////
////		// Skill 데이터 테이블에서 "Skill_Arrow" 데이터 가져오기
////		FSkillData SkillData;
////		TArray<FSkillEffectData> EffectDataArray;
////
////		if(UABGameSingleton::Get().GetSkillDataBySkillID("Skill_Arrow",SkillData) &&
////			UABGameSingleton::Get().GetSkillEffectDataBySkillID("Skill_Arrow",EffectDataArray))
////		{
////			// 초기화
////			PendingArrow->InitProjectileBySkillData(SkillData,EffectDataArray);
////
////			UE_LOG(LogTemp,Error,TEXT("Spawned Arrow Projectile"));
////		} else
////		{
////			UE_LOG(LogTemp,Error,TEXT("Failed to load Skill_Arrow data!"));
////		}
////	}
////}
//
////
////// 노티파이에서 실행 : 화살 스폰 
////void AEntityPreset::SpawnProjectile_Arrow()
////{
////	if(!NormalProjectileClass) return;
////
////	// 소켓 위치/회전 가져오기
////	FTransform SocketTransform = GetMesh()->GetSocketTransform(TEXT("ArrowSocket"),RTS_World);
////
////	// 스폰 파라미터
////	FActorSpawnParameters SpawnParams;
////	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
////	SpawnParams.Owner = this;
////	SpawnParams.Instigator = GetInstigator();
////
////	// 화살 생성
////	//PendingArrow = GetWorld()->SpawnActor<AEntityProjectile>(NormalProjectileClass,SocketTransform,SpawnParams);
////	PendingArrow = GetWorld()->SpawnActor<AEntityProjectile>(NormalProjectileClass,FVector::ZeroVector,FRotator::ZeroRotator,SpawnParams);
////	if(!PendingArrow) return;
////	
////	FVector ArrowLoc = GetMesh()->GetSocketLocation("ArrowSocket");
////	FRotator ArrowRot = GetMesh()->GetSocketRotation("ArrowSocket");
////	PendingArrow->SetActorLocation(ArrowLoc);
////	//PendingArrow->SetActorLocation(SocketTransform.GetLocation());
////	//PendingArrow->SetActorRotation(SocketTransform.GetRotation().Rotator());
////
////	// 소켓에 부착
////	PendingArrow->AttachToComponent(GetMesh(),FAttachmentTransformRules::KeepWorldTransform,TEXT("ArrowSocket"));
////
////	PendingArrow->SetActorRotation(ArrowRot);
////
////	// 자동 발사 방지
////	PendingArrow->bAutoFireOnSpawn = false;
////
////	// 발사 방향 저장 (소켓 기준 정방향)
////	ArrowDirection = GetMesh()->GetSocketRotation(TEXT("ArrowSocket")).Vector();
////
////	UE_LOG(LogTemp,Log,TEXT("[Arrow Spawned] Location: %s, Direction: %s"),
////		*SocketTransform.GetLocation().ToString(),
////		*ArrowDirection.ToString());
////
////	 //Skill 데이터 테이블에서 "Skill_Arrow" 데이터 가져오기
////	FSkillData SkillData;
////	TArray<FSkillEffectData> EffectDataArray;
////
////	if(UABGameSingleton::Get().GetSkillDataBySkillID("Skill_Arrow",SkillData) &&
////		UABGameSingleton::Get().GetSkillEffectDataBySkillID("Skill_Arrow",EffectDataArray))
////	{
////		// 초기화
////		PendingArrow->InitProjectileBySkillData(SkillData,EffectDataArray);
////	} 
////	else
////	{
////		UE_LOG(LogTemp,Error,TEXT("Failed to load Skill_Arrow data!"));
////	}
////}
//// 
//
////// 노티파이에서 실행 : 화살 발사
////void AEntityPreset::FireProjectile_Arrow()
////{
////	if(Arrow)
////	{
////		// 소켓에서 화살 분리 
////		Arrow->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
////
////		ArrowDirection = GetMesh()->GetRightVector();
////
////		// 자동 발사 막기
////		Arrow->bAutoFireOnSpawn = true;
////		Arrow->FireInDirection(ArrowDirection);
////
////		//Arrow = nullptr; // 재사용 안함
////	}
////	//if(PendingArrow)
////	//{
////	//	// 회전 원복
////	//	FRotator FixedRotation = ArrowDirection.Rotation();  		
////	//	PendingArrow->SetActorRotation(FixedRotation);
////
////	//	// 소켓에서 화살 분리 
////	//	PendingArrow->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
////
////	//	// 자동 발사 막기
////	//	PendingArrow->bAutoFireOnSpawn = true;  
////	//	PendingArrow->FireInDirection(ArrowDirection);
////
////	//	PendingArrow = nullptr; // 재사용 안함
////	//}
////}

