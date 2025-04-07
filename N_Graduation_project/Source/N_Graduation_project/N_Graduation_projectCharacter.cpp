// Copyright Epic Games, Inc. All Rights Reserved.

#include "N_Graduation_projectCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "MyPlayerStatComponent.h"
#include "PlayerSkillComponent.h"
//#include "Blueprint/UserWidget.h"
//#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"//ApplyDamage
#include "GameFramework/Character.h"//ApplyDamage
#include "CharacterStateComponent.h" //state


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AN_Graduation_projectCharacter

AN_Graduation_projectCharacter::AN_Graduation_projectCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	static ConstructorHelpers::FObjectFinder<UInputAction> MouseLeftClick = TEXT("/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/LeftClickAction.LeftClickAction'");
	if (MouseLeftClick.Object)
	{
		LeftClickAction = MouseLeftClick.Object;
	}

	// IA를 직접 지정하지 않으면 Dash 기능이 수행되지 않음 
	static ConstructorHelpers::FObjectFinder<UInputAction> DashInput = TEXT("/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/IA_Dash.IA_Dash'");
	if (DashInput.Object)
	{
		DashAction = DashInput.Object;
	}

	// Dash Curve가 존재하면 변수에 오브젝트 넣기
	const ConstructorHelpers::FObjectFinder<UCurveFloat> Curve(TEXT("/Script/Engine.CurveFloat'/Game/ThirdPerson/CV_Dash.CV_Dash'"));
	if (Curve.Succeeded())
	{
		DashCurve = Curve.Object;
	}

	// DashTimeline이 존재하면 변수에 오브젝트 넣기
	DashTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DashTimeline"));
	DashDistance = 300.0f;
	DashDirection = FVector::ZeroVector;
	DashVelocity = FVector::ZeroVector;

	// 체력 컴포넌트 추가
	PlayerStatComponent = CreateDefaultSubobject<UMyPlayerStatComponent>(TEXT("PlayerStatComponent"));
	PlayerSkillComponent = CreateDefaultSubobject<UPlayerSkillComponent>(TEXT("PlayerSkillComponent"));
	CharacterStateComponent = CreateDefaultSubobject<UCharacterStateComponent>(TEXT("CharacterStateComponent"));

	m_pMeshCom = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));

	// RootComponent 설정 -> 안 하면 m_pMeshCom nullptr 오류 발생
	if (!m_pMeshCom)
	{
		m_pMeshCom = NewObject<USkeletalMeshComponent>(this, TEXT("MeshComponent"));
		m_pMeshCom->RegisterComponent();
		RootComponent = m_pMeshCom;
	}

	bIsMoving = true;
	pastPreset = "PlayerCharacter";

	////위젯 블루프린트 클래스 찾기
	//static ConstructorHelpers::FClassFinder<UUserWidget> HUD(TEXT("WidgetBlueprint'/Game/GUI/HUD_Profile.HUD_Profile_C'"));

	//if (HUD.Succeeded())
	//{
	//	HUDClass = HUD.Class; // 위젯 클래스 설정
	//}
}

void AN_Graduation_projectCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//HUDWidget = CreateWidget(GetWorld()->GetFirstPlayerController(), HUDClass);
	//HUDWidget->AddToViewport();

	FOnTimelineFloat DashCallback;
	currentPreset = "PlayerCharacter";

	//UpdateEntityData();
	//currentPreset = "WildBoar";

	// Dash가 수행될 때 Callback 되는 함수 DashInterpReturn 지정
	DashCallback.BindUFunction(this, FName("DashInterpReturn"));

	// 타임라인 반복 false 설정 
	DashTimeline->SetLooping(false);
	// DashCurve에 따라 타임라인/Callback 수행
	DashTimeline->AddInterpFloat(DashCurve, DashCallback);
	// 타임라인 길이 설정
	DashTimeline->SetTimelineLength(0.2f);

	//if (PlayerStatComponent)
	//{
	//	PlayerStatComponent->OnHPIsZero.AddDynamic(this, &AN_Graduation_projectCharacter::OnPlayerDead);
	//}
}
void AN_Graduation_projectCharacter::Tick(float DeltaTime)
{
	FVector Velocity = GetVelocity();
	float Speed = Velocity.Size(); // 현재 속도
	PlayerSkillComponent->VisibleHitBox("Skill_Slash");
	UCharacterStateComponent* StateComp = FindComponentByClass<UCharacterStateComponent>();
	if (!StateComp)
	{
		return; // StateComp가 없으면 함수 종료
	}

	// Speed가 일정 임계값보다 크면 이동 중
	if (Speed > 0.1f)
	{
		if (StateComp->GetCurrentState() != ECharacterState::Move)
		{
			StateComp->ChangeState(ECharacterState::Move);  // Move 상태로 변경
		}
	}
	else  // 속도가 0이면 Idle 상태
	{
		if (StateComp->GetCurrentState() != ECharacterState::Idle)
		{
			StateComp->ChangeState(ECharacterState::Idle);  // Idle 상태로 변경
		}
	}

	RotateCharacterToCursor();
}

//////////////////////////////////////////////////////////////////////////
// Input

void AN_Graduation_projectCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AN_Graduation_projectCharacter::Move);

		// Looking
		//EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AN_Graduation_projectCharacter::Look);

		// Dash
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &AN_Graduation_projectCharacter::DashCheck);

		// NomalSkill
		EnhancedInputComponent->BindAction(LeftClickAction, ETriggerEvent::Triggered, this, &AN_Graduation_projectCharacter::NomalSkillAction);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}
void AN_Graduation_projectCharacter::NomalSkillAction(const FInputActionValue& Value)
{
	if (CharacterStateComponent->CurrentState == ECharacterState::Action|| CharacterStateComponent->CurrentState == ECharacterState::Dash) {
		return;
	}
	if(PlayerSkillComponent->CanUseNomalSkill==true) {
		StartAction();
		//UE_LOG(LogTemp, Error, TEXT("NomalSkillAction"));
		PlayerSkillComponent->NomalSkillPlay("Skill_FireBall");
		//EndAction();
	}
}

void AN_Graduation_projectCharacter::Move(const FInputActionValue& Value)
{
	UCharacterStateComponent* StateComp = FindComponentByClass<UCharacterStateComponent>();

	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);

		if (StateComp)
		{
			FString StateString = UEnum::GetValueAsString(StateComp->CurrentState);
			StateComp->ChangeState(ECharacterState::Move);
		}
	}
}

//void AN_Graduation_projectCharacter::Look(const FInputActionValue& Value)
//{
//	// input is a Vector2D
//	FVector2D LookAxisVector = Value.Get<FVector2D>();
//
//	if (Controller != nullptr)
//	{
//		// add yaw and pitch input to controller
//		AddControllerYawInput(LookAxisVector.X);
//		AddControllerPitchInput(LookAxisVector.Y);
//	}
//}
void AN_Graduation_projectCharacter::RotateCharacterToCursor()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController) return;

	if (PlayerController->DeprojectMousePositionToWorld(MouseWorldPosition, MouseWorldDirection))
	{
		// Raycast to find the hit location under the cursor
		FHitResult HitResult;
		FVector Start = MouseWorldPosition;
		FVector End = Start + (MouseWorldDirection * 10000.0f);

		if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility))
		{
			FVector TargetLocation = HitResult.Location;
			FVector CharacterLocation = GetActorLocation();

			// Calculate the desired rotation
			FRotator LookAtRotation = (TargetLocation - CharacterLocation).Rotation();
			LookAtRotation.Pitch = 0.0f; // Lock Pitch
			LookAtRotation.Roll = 0.0f; // Lock Roll

			// Smooth rotation
			FRotator CurrentRotation = GetActorRotation();
			FRotator NewRotation = FMath::RInterpTo(CurrentRotation, LookAtRotation, GetWorld()->GetDeltaSeconds(), 10.0f);

			SetActorRotation(NewRotation);
		}
	}

}

void AN_Graduation_projectCharacter::DashCheck(const FInputActionValue& Value)
{
	UE_LOG(LogTemplateCharacter, Error, TEXT("Dash 바인딩"));
	// 대시 상태로 변경
	UCharacterStateComponent* StateComp = FindComponentByClass<UCharacterStateComponent>();
	StateComp->ChangeState(ECharacterState::Dash);
	CharacterStateComponent->ApplyActionRestrictions();
	

	// 마지막 입력이 ZeroVector(중립)가 아니면 실행 -> 캐릭터가 정지 중엔 실행되지 않음(반드시 대시로 이동하고 싶은 방향쪽 방향키를 눌러야 대시 발동(기획서대로 수정 필요))
	if (GetCharacterMovement()->GetLastInputVector() != FVector::ZeroVector)
	{
		FHitResult HitResult;
		// 상태 업데이트
	//	StartAction();	
		CharacterStateComponent->isDash = true;

		// LineTracer를 이용해 현재 액터의 위치와 마지막 입력이 가해졌던 방향(마지막 움직임의 이동방향)에 DashDistance를 곱해 나온 위치로 Dash 
		bool IsHit = GetWorld()->LineTraceSingleByChannel(HitResult,
			GetActorLocation(),
			GetActorLocation() + (GetCharacterMovement()->GetLastInputVector() * DashDistance),
			ECollisionChannel::ECC_Visibility);
		// Dash가 발동되어 최종적으로 이동할 위치에 액터 또는 충돌 가능한 무언가가 존재한다면
		if (IsHit) {
			// 충돌한 객체의 위치값에 캐릭터의 몸 값(55.0f)을 빼서 이동
			Dash(HitResult.Location + (GetCharacterMovement()->GetLastInputVector() * -55.0f), GetActorForwardVector());
		}
		// 존재하지 않는다면 DashDistance만큼 이동 
		else {
			Dash(GetActorLocation() + (GetCharacterMovement()->GetLastInputVector() * DashDistance), GetActorForwardVector());
		}
		EndAction();
		CharacterStateComponent->isDash = false;

	}
}

void AN_Graduation_projectCharacter::Dash(const FVector DashDir, const FVector DashVel)
{
	UCharacterStateComponent* StateComp = FindComponentByClass<UCharacterStateComponent>();

	DashDirection = DashDir;
	DashVelocity = DashVel;
	DashTimeline->PlayFromStart();
//	EndDash();
	/*currentPreset = "Inpermon";
	UpdateEntityData();
	//속도 변하는지 체크하려고
	*/
	currentPreset = "WildBoar"; //임시로	
	SetPreset(currentPreset);

	UpdateEntityData();
	/*FString HPText = FString::Printf(TEXT("HP: %f"), PlayerStatComponent->CurrentHP);
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, HPText);*/
}

void AN_Graduation_projectCharacter::DashInterpReturn(float value)
{
	// Dash 키 입력 -> DeshCheck 바인딩 -> Dash 수행 -> 타임라인 실행 -> DashCurve에 따라 Callback 함수 DashInterpReturn 바인딩 -> 로케이션 
	SetActorLocation(FMath::Lerp(GetActorLocation(), DashDirection, value));

}

// 데미지를 받았을 때 호출하는 함수
float AN_Graduation_projectCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("IsDefending: %s"), PlayerSkillComponent->IsDefending ? TEXT("true") : TEXT("false"));
	if (PlayerSkillComponent->IsDefending == true) {
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Damage Blocked by Defense Skill"));
		//IsInvincible = false;
		return 0.0f;//무적상태라면 리턴.

	}
	else
	{
		PlayerStatComponent->ApplyDamage(DamageAmount);
		On_invincibility_Implementation();
		IsInvincible = false;
		// 데미지 로그 출력	
		float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	//	UE_LOG(LogTemp, Error, TEXT("avocado take damage %f"), DamageAmount);

		//PlayerSkillComponent->OnDefenseSkill(3.0);

		return FinalDamage;
	}
}

void AN_Graduation_projectCharacter::On_invincibility_Implementation()
{
	if (PlayerSkillComponent && !IsInvincible)
	{
		// 무적 상태 활성화
		IsInvincible = true;
		if (IsInvincible) {
			UE_LOG(LogTemp, Log, TEXT("IsInvincible"));
		}
		// PlayerSkillComponent에서 방어 스킬을 실행
		PlayerSkillComponent->OnDefenseSkill(1.0f);
		// 깜박이기구현->BP
	}
}

void AN_Graduation_projectCharacter::UpdateEntityData()
{
	if (UABGameSingleton::Get().GetEntityDataByGroupID(currentPreset, EntityData))
	{
		SetActorLabel(EntityData.EntityName);
		SetMoveSpeed(EntityData.MoveSpeed);
		SetPreset(EntityData.PresetReference);
		UE_LOG(LogTemp, Error, TEXT("!Entity Name: %s, HP: %d, Move Speed: %d"),
			*EntityData.EntityName, EntityData.HP, EntityData.MoveSpeed);
	}
	// currentPreset!=클릭한 버튼의 캐릭터이름 이면..
	if (pastPreset != currentPreset) {
		UE_LOG(LogTemp, Error, TEXT("= pastPreset != currentPreset"));
		PlayerStatComponent->TransformToEntity(EntityData.EntityGroupID,EntityData.HP, EntityData.TransManaCost);
		pastPreset = currentPreset;
	}
}

void AN_Graduation_projectCharacter::SetMoveSpeed(int32 MoveSpeed)
{
	UE_LOG(LogTemp, Error, TEXT("!currentSpeed: %d"), currentSpeed);

	currentSpeed = MoveSpeed;
	GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	if (currentSpeed == 0)
	{
		GetCharacterMovement()->StopMovementImmediately();
	}
}
void AN_Graduation_projectCharacter::StartAction()
{
	UE_LOG(LogTemp, Error, TEXT("!StartAction"));

	// 플레이어 컨트롤러 가져오기
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	//if (PlayerController)
	//{
	//	//PlayerController->DisableInput(PlayerController); // 입력 비활성화
	//	GetCharacterMovement()->MaxWalkSpeed = 0;
	//	UE_LOG(LogTemp, Error, TEXT("Yes PlayerController!"));
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Error, TEXT("No PlayerController"));
	//}   
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(DefaultMappingContext);
			UE_LOG(LogTemp, Error, TEXT("Input Mapping Context Removed"));
		}
	}

}
void AN_Graduation_projectCharacter::EndAction()
{
	UCharacterStateComponent* StateComp = FindComponentByClass<UCharacterStateComponent>();
	StateComp->ChangeState(ECharacterState::Idle);
	CharacterStateComponent->isAction = false;

	// 기존 속도로 이동 
	UE_LOG(LogTemp, Error, TEXT("!EndAction"));
	// Action 상태 종료 시 입력 활성화
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
			UE_LOG(LogTemp, Error, TEXT("Input Mapping Context Restored"));
		}

		UE_LOG(LogTemp, Error, TEXT("Yes Action PlayerController!"));
	}
}
void AN_Graduation_projectCharacter::OnPlayerDead()
{
	UCharacterStateComponent* StateComp = FindComponentByClass<UCharacterStateComponent>();
	if (StateComp)
	{
		FString StateString = UEnum::GetValueAsString(StateComp->CurrentState);
		StateComp->ChangeState(ECharacterState::Dead);
	}
	// HP가 0이 되었을 때 처리할 로직
	UE_LOG(LogTemp, Warning, TEXT("Player is dead!"));
	// 여기서 플레이어 죽음 처리 (예: 애니메이션, UI 변경 등)
}

void AN_Graduation_projectCharacter::SetPreset(FString PresetReference)
{
	currentPreset = PresetReference;

	// 프리셋 이름마다 메시 에셋 파일 할당
	if (currentPreset == "PCPreset.uasset")
	{
		// 에디터 실행 시 문제없이 메시를 로드하기 위해 FSoftObjectPath를 사용해 비동기 로딩 
		FSoftObjectPath MeshPath(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny"));

		USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshPath.TryLoad());
		GetMesh()->SetSkeletalMesh(LoadedMesh);

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
		GetMesh()->SetSkeletalMesh(LoadedMesh);

		if (LoadedMesh)
		{
			m_pMeshCom->SetSkeletalMesh(LoadedMesh);  // SkeletalMesh는 Skel_MeshCom을 사용
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("I'm Here")));
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


/* //나중에 테스터에
void AN_Graduation_projectCharacter::DealDamageToPlayer()
{
	UE_LOG(LogTemp, Error, TEXT("50 Damage"));

	//0번 플레이어를 가져온다
	ACharacter* TargetCharacter = Cast<ACharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	float DamageAmount = 50.0f;
	// TargetCharacter에서 GetController를 호출
	AController* InstigatorController = TargetCharacter->GetController();
	AActor* DamageCauser = this; // 데미지를 주는 액터
	TSubclassOf<UDamageType> DamageType = UDamageType::StaticClass(); // 기본 데미지 타입

	// 데미지 적용
	UGameplayStatics::ApplyDamage(TargetCharacter, DamageAmount, InstigatorController, DamageCauser, DamageType);

}
*/