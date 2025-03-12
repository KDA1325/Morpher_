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
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"

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

	//위젯 블루프린트 클래스 찾기
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetFinder(TEXT("WidgetBlueprint'/Game/Entity/BP/Character_HealthBar.Character_HealthBar_C'"));

	if (WidgetFinder.Succeeded())
	{
		CharacterHealthBarWidgetClass = WidgetFinder.Class; // 위젯 클래스 설정
	}
}

void AN_Graduation_projectCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	SpawnWidget();

	FOnTimelineFloat DashCallback;

	//On_invincibility();//피격 테스트용

	// Dash가 수행될 때 Callback 되는 함수 DashInterpReturn 지정
	DashCallback.BindUFunction(this, FName("DashInterpReturn"));

	// 타임라인 반복 false 설정 
	DashTimeline->SetLooping(false);
	// DashCurve에 따라 타임라인/Callback 수행
	DashTimeline->AddInterpFloat(DashCurve, DashCallback);
	// 타임라인 길이 설정
	DashTimeline->SetTimelineLength(0.2f);

}

void AN_Graduation_projectCharacter::SpawnWidget()
{
	if (IsValid(CharacterHealthBarWidgetClass))
	{
		CharacterHealthBarWidget = CreateWidget<UUserWidget>(GetWorld(), CharacterHealthBarWidgetClass);

		if (IsValid(CharacterHealthBarWidget))
		{
			CharacterHealthBarWidget->AddToViewport();
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Character HealthBar Widget Added!"));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Failed to Create Character HealthBar Widget!"));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("CharacterHealthBarWidgetClass Not Set"));
	}
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
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AN_Graduation_projectCharacter::Look);

		// Dash
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &AN_Graduation_projectCharacter::DashCheck);

	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AN_Graduation_projectCharacter::Move(const FInputActionValue& Value)
{
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
	}
}

void AN_Graduation_projectCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AN_Graduation_projectCharacter::DashCheck(const FInputActionValue& Value)
{
	UE_LOG(LogTemplateCharacter, Error, TEXT("Dash 바인딩"));

	// 마지막 입력이 ZeroVector(중립)가 아니면 실행 -> 캐릭터가 정지 중엔 실행되지 않음(반드시 대시로 이동하고 싶은 방향쪽 방향키를 눌러야 대시 발동(기획서대로 수정 필요))
	if (GetCharacterMovement()->GetLastInputVector() != FVector::ZeroVector)
	{
		FHitResult HitResult;

		// LineTracer를 이용해 현재 액터의 위치와 마지막 입력이 가해졌던 방향(마지막 움직임의 이동방향)에 DashDistance를 곱해 나온 위치로 Dash 
		bool IsHit = GetWorld()->LineTraceSingleByChannel(HitResult,
			GetActorLocation(),
			GetActorLocation() + (GetCharacterMovement()->GetLastInputVector() * DashDistance),
			ECollisionChannel::ECC_Visibility);

		// Dash가 발동되어 최종적으로 이동할 위치에 액터 또는 충돌 가능한 무언가가 존재한다면
		if (IsHit)
			// 충돌한 객체의 위치값에 캐릭터의 몸 값(55.0f)을 빼서 이동
			Dash(HitResult.Location + (GetCharacterMovement()->GetLastInputVector() * -55.0f), GetActorForwardVector());
		// 존재하지 않는다면 DashDistance만큼 이동 
		else
			Dash(GetActorLocation() + (GetCharacterMovement()->GetLastInputVector() * DashDistance), GetActorForwardVector());
	}
}

void AN_Graduation_projectCharacter::Dash(const FVector DashDir, const FVector DashVel)
{
	DashDirection = DashDir;
	DashVelocity = DashVel;
	DashTimeline->PlayFromStart();
}

void AN_Graduation_projectCharacter::DashInterpReturn(float value)
{
	// Dash 키 입력 -> DeshCheck 바인딩 -> Dash 수행 -> 타임라인 실행 -> DashCurve에 따라 Callback 함수 DashInterpReturn 바인딩 -> 로케이션 
	SetActorLocation(FMath::Lerp(GetActorLocation(), DashDirection, value));
}


// 데미지를 받았을 때 호출하는 함수
float AN_Graduation_projectCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (PlayerSkillComponent->IsDefending) return 0.0f;//무적상태라면 리턴.

	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	//MyStat->SetDamage(FinalDamage); //데미지 설정.

	On_invincibility();
	if (PlayerSkillComponent && PlayerSkillComponent->IsDefending)
	{
		DamageAmount = 0.0f; // 방어 중이라면 데미지 0
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Damage Blocked by Defense Skill"));
		}
	}
	//bDoingSomething = true;
//	MyAnim->PlayDamagedMontage();

	return FinalDamage;
}

void AN_Graduation_projectCharacter::On_invincibility()
{
	if (PlayerSkillComponent && !IsInvincible)
	{
		// 무적 상태 활성화
		IsInvincible = true;

		// PlayerSkillComponent에서 방어 스킬을 실행
		PlayerSkillComponent->OnDefenseSkill(1.0f);

		// 깜박이기구현..해야함

	}
}
