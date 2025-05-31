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
#include "Kismet/GameplayStatics.h"//ApplyDamage
#include "GameFramework/Character.h"//ApplyDamage
#include "CharacterStateComponent.h" //state
#include "MySaveGame.h"
#include "MyGameInstance.h"
#include "Components/SphereComponent.h"
#include "Engine/EngineTypes.h"              // FDamageEvent
#include "GameFramework/DamageType.h"        // UDamageType
#include "NormalAttackDamageType.h"          // 커스텀 데미지 타입
#include "Engine/DamageEvents.h"
DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AN_Graduation_projectCharacter

AN_Graduation_projectCharacter::AN_Graduation_projectCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f,96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f,500.0f,0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 1000.0;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 3000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom,USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	static ConstructorHelpers::FObjectFinder<UInputAction> MouseLeftClick = TEXT("/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/LeftClickAction.LeftClickAction'");
	if(MouseLeftClick.Object)
	{
		LeftClickAction = MouseLeftClick.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> MouseRightClick = TEXT("/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/RightClickAction.RightClickAction'");
	if(MouseLeftClick.Object)
	{
		RightClickAction = MouseRightClick.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> MouseRightReleased = TEXT("/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/RightReleasedClickAction.RightReleasedClickAction'");
	if(MouseRightReleased.Object)
	{
		RightReleasedClickAction = MouseRightReleased.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> Tab = TEXT("/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/IA_Tab.IA_Tab'");
	if(Tab.Object)
	{
		PieMenuAction = Tab.Object;
	}

	// IA를 직접 지정하지 않으면 Dash 기능이 수행되지 않음 
	static ConstructorHelpers::FObjectFinder<UInputAction> DashInput = TEXT("/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/IA_Dash.IA_Dash'");
	if(DashInput.Object)
	{
		DashAction = DashInput.Object;
	}

	// Dash Curve가 존재하면 변수에 오브젝트 넣기
	const ConstructorHelpers::FObjectFinder<UCurveFloat> Curve(TEXT("/Script/Engine.CurveFloat'/Game/ThirdPerson/CV_Dash.CV_Dash'"));
	if(Curve.Succeeded())
	{
		DashCurve = Curve.Object;
	}

	// DashTimeline이 존재하면 변수에 오브젝트 넣기
	DashTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DashTimeline"));
	DashDistance = 300.0f;
	DashDirection = FVector::ZeroVector;
	DashVelocity = FVector::ZeroVector;

	// 체력 컴포넌트 추가
	WidgetActor = CreateDefaultSubobject<UWidgetActor>(TEXT("WidgetActorComponent"));
	PlayerStatComponent = CreateDefaultSubobject<UMyPlayerStatComponent>(TEXT("PlayerStatComponent"));
	PlayerSkillComponent = CreateDefaultSubobject<UPlayerSkillComponent>(TEXT("PlayerSkillComponent"));
	CharacterStateComponent = CreateDefaultSubobject<UCharacterStateComponent>(TEXT("CharacterStateComponent"));
	m_pMeshCom = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));

	// RootComponent 설정 -> 안 하면 m_pMeshCom nullptr 오류 발생
	if(!m_pMeshCom)
	{
		m_pMeshCom = NewObject<USkeletalMeshComponent>(this,TEXT("MeshComponent"));
		m_pMeshCom->RegisterComponent();
		RootComponent = m_pMeshCom;
	}

	bIsMoving = true;
	bcanPie=true;
}

void AN_Graduation_projectCharacter::BeginPlay()
{
	// Call the base class    
	Super::BeginPlay();
	m_pMeshCom = GetMesh();

	FOnTimelineFloat DashCallback;
	SpawnHitBoxAtSocket("AttachHitBox");

	//UE_LOG(LogTemp,Log,TEXT("캐릭터 BeginPlay실시, player: %s"),*currentPreset);
	PlayerSword = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("Player_sword")));
	SkeletonBow =  Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("Bow")));
	SkeletonShield = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("Shield")));
	SkeletonSword = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("Skel_Sword")));
	if(SkeletonSword){
		SkeletonBow ->SetHiddenInGame(true);
		SkeletonShield ->SetHiddenInGame(true);
		SkeletonSword->SetHiddenInGame(true);
		//UE_LOG(LogTemp,Log,TEXT("스켈레톤칼 방패 있음"))}
	}
	
	isDead=false;
	// Dash가 수행될 때 Callback 되는 함수 DashInterpReturn 지정
	DashCallback.BindUFunction(this,FName("DashInterpReturn"));

	// 타임라인 반복 false 설정 
	DashTimeline->SetLooping(false);
	// DashCurve에 따라 타임라인/Callback 수행
	DashTimeline->AddInterpFloat(DashCurve,DashCallback);
	// 타임라인 길이 설정
	DashTimeline->SetTimelineLength(0.2f);

	USkeletalMeshComponent* MeshComponent = GetMesh();
	if(MeshComponent)
	{
		InvincibleOriginalMaterial = MeshComponent->GetMaterial(0);
	}

	HitMaterial = LoadObject<UMaterialInterface>(nullptr,TEXT("MaterialInterface'/Game/UI/Materials/Hit.Hit'"));
	if(!HitMaterial)
	{
		UE_LOG(LogTemp,Error,TEXT("Failed to load HitMaterial!"));
	}

	UE_LOG(LogTemp,Error,TEXT("ChangePreset 캐릭터 BeginPlay"));


	auto* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if(!MyGameInstance) return;
	else{
		MyGameInstance->LoadGame();
	}
}
void AN_Graduation_projectCharacter::Tick(float DeltaTime)
{
	FVector Velocity = GetVelocity();
	float Speed = Velocity.Size(); // 현재 속도
	//	UE_LOG(LogTemp, Log, TEXT("IsInvincible: %s"), IsInvincible ? TEXT("true") : TEXT("false"));

		/*if (GetMesh()->GetAnimInstance()) {
			UE_LOG(LogTemp, Warning, TEXT("qoqo AnimInstance: %s"), *GetMesh()->GetAnimInstance()->GetName());
		}*/

	UCharacterStateComponent* StateComp = FindComponentByClass<UCharacterStateComponent>();
	if(!StateComp)
	{
		return; // StateComp가 없으면 함수 종료
	}
	//PlayerSkillComponent->MeasureDistanceToMonster();
	// Speed가 일정 임계값보다 크면 이동 중
	if(Speed > 0.1f)
	{
		if(StateComp->GetCurrentState() != ECharacterState::Move)
		{
			StateComp->ChangeState(ECharacterState::Move);  // Move 상태로 변경
		}
	} else  // 속도가 0이면 Idle 상태
	{
		if(StateComp->GetCurrentState() != ECharacterState::Idle)
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
	if(APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext,3);
		}
	}

	// Set up action bindings
	if(UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction,ETriggerEvent::Started,this,&ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction,ETriggerEvent::Completed,this,&ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,this,&AN_Graduation_projectCharacter::Move);

		// Looking
		//EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AN_Graduation_projectCharacter::Look);

		// Dash
		EnhancedInputComponent->BindAction(DashAction,ETriggerEvent::Triggered,this,&AN_Graduation_projectCharacter::DashCheck);

		// NomalSkill
		EnhancedInputComponent->BindAction(LeftClickAction,ETriggerEvent::Started,this,&AN_Graduation_projectCharacter::NomalSkillAction);
		//special Skill
		EnhancedInputComponent->BindAction(RightClickAction,ETriggerEvent::Triggered,this,&AN_Graduation_projectCharacter::SpecialSkillAction);
		EnhancedInputComponent->BindAction(RightReleasedClickAction,ETriggerEvent::Completed,this,&AN_Graduation_projectCharacter::EndShield);

		EnhancedInputComponent->BindAction(PieMenuAction,ETriggerEvent::Started,this,&AN_Graduation_projectCharacter::OnPieMenuPressed);
		EnhancedInputComponent->BindAction(PieMenuAction,ETriggerEvent::Completed,this,&AN_Graduation_projectCharacter::OnPieMenuReleased);

	} else
	{
		//	UE_LOG(LogTemplateCharacter,Error,TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."),*GetNameSafe(this));
	}
}
void AN_Graduation_projectCharacter::SpecialSkillAction(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Blue,TEXT("마우스 우클릭"));
	FVector Location = GetOwner()->GetActorLocation();
	//UE_LOG(LogTemp,Warning,TEXT("캐릭터 위치: X=%.2f, Y=%.2f, Z=%.2f"),Location.X,Location.Y,Location.Z);
	if(CharacterStateComponent->CurrentState == ECharacterState::Action || CharacterStateComponent->CurrentState == ECharacterState::Dash) {
		return;
	}
	if(PlayerSkillComponent->CanUseSpecialSkill == true) {
		PlayerSkillComponent->SpecialSkillPlay(SpecialSkill);
		//PlaySpecial = true;			
		UE_LOG(LogTemp,Warning,TEXT("실드 우클릭"));

	} else GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Blue,TEXT("마우스 클릭 실패"));
}
void AN_Graduation_projectCharacter::EndShield()
{		
	if(currentPreset  == "SkeletonWarriorPreset.uasset"){
		PlayerSkillComponent->OffDefenseSkill();
		UE_LOG(LogTemp,Warning,TEXT("실드 해제됨"));
	}
}
void AN_Graduation_projectCharacter::NomalSkillAction(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Green,TEXT("마우스 좌클릭"));

	if(CharacterStateComponent->CurrentState == ECharacterState::Action || CharacterStateComponent->CurrentState == ECharacterState::Dash) {
		return;
	}
	if(PlayerSkillComponent->CanUseNomalSkill == true) {
		PlayerSkillComponent->NomalSkillPlay(NomalSkill);
		//UE_LOG(LogTemp,Warning,TEXT("CanUseNomalSkill: %s"),PlayerSkillComponent->CanUseNomalSkill ? TEXT("true") : TEXT("false"));

		//	PlayNomal = true;
	} else GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Blue,TEXT("마우스 클릭 실패"));
	//	UE_LOG(LogTemp, Warning, TEXT("CanUseNomalSkill: %s"), PlayerSkillComponent->CanUseNomalSkill ? TEXT("true") : TEXT("false"));

}

void AN_Graduation_projectCharacter::Move(const FInputActionValue& Value)
{
	if(bCanMove){
		UCharacterStateComponent* StateComp = FindComponentByClass<UCharacterStateComponent>();

		// input is a Vector2D
		FVector2D MovementVector = Value.Get<FVector2D>();

		if(Controller != nullptr)
		{
			// find out which way is forward
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0,Rotation.Yaw,0);

			// get forward vector
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

			// get right vector
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			// add movement 
			AddMovementInput(ForwardDirection,MovementVector.Y);
			AddMovementInput(RightDirection,MovementVector.X);

			if(StateComp)
			{
				FString StateString = UEnum::GetValueAsString(StateComp->CurrentState);
				StateComp->ChangeState(ECharacterState::Move);
			}
		}
	}
}

void AN_Graduation_projectCharacter::OnPieMenuPressed()
{
	if(WidgetActor&&bCanMove)
	{
		WidgetActor->ShowPieMenu();
	}
}

void AN_Graduation_projectCharacter::OnPieMenuReleased()
{
	if(WidgetActor&&bCanMove)
	{
		WidgetActor->HidePieMenu();
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
//}ddd

void AN_Graduation_projectCharacter::RotateCharacterToCursor()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if(!PlayerController) return;

	if(!(CharacterStateComponent->CurrentState == ECharacterState::Action))
	{
		if(PlayerController->DeprojectMousePositionToWorld(MouseWorldPosition,MouseWorldDirection))
		{
			// Raycast to find the hit location under the cursor
			FHitResult HitResult;
			FVector Start = MouseWorldPosition;
			FVector End = Start + (MouseWorldDirection * 10000.0f);

			if(GetWorld()->LineTraceSingleByChannel(HitResult,Start,End,ECC_Visibility))
			{
				FVector TargetLocation = HitResult.Location;
				FVector CharacterLocation = GetActorLocation();

				// Calculate the desired rotation
				//FRotator LookAtRotation = (TargetLocation - CharacterLocation).Rotation();
				//LookAtRotation.Pitch = 0.0f; // Lock Pitch
				//LookAtRotation.Roll = 0.0f; // Lock Roll

				// Z 값 고정 (수평 방향만 계산)
				TargetLocation.Z = CharacterLocation.Z;

				// Calculate the desired rotation
				FRotator LookAtRotation = (TargetLocation - CharacterLocation).Rotation();

				// Smooth rotation
				FRotator CurrentRotation = GetActorRotation();
				FRotator NewRotation = FMath::RInterpTo(CurrentRotation,LookAtRotation,GetWorld()->GetDeltaSeconds(),10.0f);

				SetActorRotation(NewRotation);
			}
		}
	}
}

void AN_Graduation_projectCharacter::DashCheck(const FInputActionValue& Value)
{
	UE_LOG(LogTemplateCharacter,Error,TEXT("Dash 바인딩"));
	// 대시 상태로 변경
	UCharacterStateComponent* StateComp = FindComponentByClass<UCharacterStateComponent>();
	StateComp->ChangeState(ECharacterState::Dash);
	CharacterStateComponent->ApplyActionRestrictions();


	// 마지막 입력이 ZeroVector(중립)가 아니면 실행 -> 캐릭터가 정지 중엔 실행되지 않음(반드시 대시로 이동하고 싶은 방향쪽 방향키를 눌러야 대시 발동(기획서대로 수정 필요))
	if(GetCharacterMovement()->GetLastInputVector() != FVector::ZeroVector)
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
		if(IsHit) {
			// 충돌한 객체의 위치값에 캐릭터의 몸 값(55.0f)을 빼서 이동
			Dash(HitResult.Location + (GetCharacterMovement()->GetLastInputVector() * -55.0f),GetActorForwardVector());
		}
		// 존재하지 않는다면 DashDistance만큼 이동 
		else {
			Dash(GetActorLocation() + (GetCharacterMovement()->GetLastInputVector() * DashDistance),GetActorForwardVector());
		}
		EndAction();
		CharacterStateComponent->isDash = false;

	}
}

void AN_Graduation_projectCharacter::Dash(const FVector DashDir,const FVector DashVel)
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

		/*FString HPText = FString::Printf(TEXT("HP: %f"), PlayerStatComponent->CurrentHP);
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, HPText);*/
}

void AN_Graduation_projectCharacter::DashInterpReturn(float value)
{
	// Dash 키 입력 -> DeshCheck 바인딩 -> Dash 수행 -> 타임라인 실행 -> DashCurve에 따라 Callback 함수 DashInterpReturn 바인딩 -> 로케이션 
	SetActorLocation(FMath::Lerp(GetActorLocation(),DashDirection,value));

}

// 데미지를 받았을 때 호출하는 함수
float AN_Graduation_projectCharacter::TakeDamage(float DamageAmount,FDamageEvent const& DamageEvent,AController* EventInstigator,AActor* DamageCauser)
{
	bool bFromNormalHitBox=false;
	if(DamageEvent.DamageTypeClass)
	{
		UDamageType* DamageTypeCDO = DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>();

		// 예시: NormalAttackDamageType 인지 확인
		if(DamageTypeCDO->IsA(UNormalAttackDamageType::StaticClass()))
		{
			bFromNormalHitBox=true;
			UE_LOG(LogTemp,Warning,TEXT(">>> 받은 데미지 타입: NormalAttackDamageType"));
		} else
		{
			bFromNormalHitBox=false;
			UE_LOG(LogTemp,Warning,TEXT(">>> 받은 데미지 타입: %s"),*DamageTypeCDO->GetClass()->GetName());
		}
	}
	if(TestMode ==true){

		PlayerSkillComponent->CanUseNomalSkill = true;
		On_invincibility_Implementation();
		// 데미지 로그 출력	
		float FinalDamage = Super::TakeDamage(DamageAmount,DamageEvent,EventInstigator,DamageCauser);
		UE_LOG(LogTemp,Error,TEXT("무적 모드 Player TakeDamage  %f"),DamageAmount);

		return FinalDamage;
	}
	else {
		//UE_LOG(LogTemp, Warning, TEXT("IsDefending: %s"), PlayerSkillComponent->IsDefending ? TEXT("true") : TEXT("false"));
		if(IsInvincible||(PlayerSkillComponent->IsDefending==true && bFromNormalHitBox==true)) {
			UE_LOG(LogTemp,Error,TEXT("Player TakeDamage 데미지 받지 않음"));
			//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Damage Blocked by Defense Skill"));
			PlayerSkillComponent->CanUseNomalSkill = true;

				return 0.0f;//무적상태라면 리턴.

		} else
		{
			PlayerSkillComponent->CanUseNomalSkill = true;
			PlayerStatComponent->ApplyDamage(DamageAmount);
			On_invincibility_Implementation();
			// 데미지 로그 출력	
			float FinalDamage = Super::TakeDamage(DamageAmount,DamageEvent,EventInstigator,DamageCauser);
			UE_LOG(LogTemp,Error,TEXT("Player TakeDamage  %f"),DamageAmount);

			return FinalDamage;
		}
	}


}
void AN_Graduation_projectCharacter::On_invincibility_Implementation()
{
	if(PlayerSkillComponent && !IsInvincible)
	{
		IsInvincible = true;

		//		PlayerSkillComponent->OnDefenseSkill();

		USkeletalMeshComponent* MeshComponent = GetMesh();
		if(MeshComponent)
		{
			// 무적 상태에서 머터리얼을 변경할 때만 처리
			if(InvincibleOriginalMaterial == nullptr)
			{
				// 현재 머터리얼 저장 (변신 시 머터리얼 저장과 구분)
				InvincibleOriginalMaterial = MeshComponent->GetMaterial(0);
			}

			// 첫 번째 깜빡임 (HitMaterial로 바꾸기)
			ToggleMaterial(true);

			// 0.25초 후에 원래 색으로
			GetWorld()->GetTimerManager().SetTimer(FlashTimerHandle1,[this]() { ToggleMaterial(false); },0.25f,false);

			// 0.5초 후 다시 Hit로
			GetWorld()->GetTimerManager().SetTimer(FlashTimerHandle2,[this]() { ToggleMaterial(true); },0.5f,false);

			// 0.75초 후 다시 원래 색
			GetWorld()->GetTimerManager().SetTimer(FlashTimerHandle3,[this]() { ToggleMaterial(false); },0.75f,false);

			// 마지막으로 1초 후 무적 종료
			GetWorld()->GetTimerManager().SetTimer(FlashTimerHandle4,[this](){ IsInvincible = false; },1.0f,false);
		}
	}
}

// ToggleMaterial 수정
void AN_Graduation_projectCharacter::ToggleMaterial(bool bUseHitMaterial)
{
	if(USkeletalMeshComponent* MeshComponent = GetMesh()){

		if(isDead==false)
		{
			UMaterialInterface* TargetMaterial = bUseHitMaterial ? HitMaterial : InvincibleOriginalMaterial;

			if(!TargetMaterial)
			{
				//UE_LOG(LogTemp,Error,TEXT("TargetMaterial is null! (bUseHitMaterial: %s)"),bUseHitMaterial ? TEXT("true") : TEXT("false"));
				return;
			}

			MeshComponent->SetMaterial(0,TargetMaterial);

			Tcount++;
			if(Tcount > 3)
			{
				// 필요 없다면 아래 줄 생략 (주의: nullptr 초기화 금지)
				// InvincibleOriginalMaterial = nullptr;
				Tcount = 0;
			}

		}
	} else	{
		//	UE_LOG(LogTemp,Error,TEXT("MeshComponent is null!"));
		return;
	}
}

void AN_Graduation_projectCharacter::UpdateEntityData()
{
	//	UE_LOG(LogTemp,Error,TEXT("Loadgame UpdateEntityData 실행됨"));


	if(UABGameSingleton::Get().GetEntityDataByGroupID(currentPreset,EntityData))
	{
		if(PlayerStatComponent->CurrentMana >= EntityData.TransManaCost) 		OkTrans = true;
		if(!(PlayerStatComponent->CurrentMana >= EntityData.TransManaCost))		OkTrans = false;

		InvincibleOriginalMaterial = nullptr;
		//SetActorLabel(EntityData.EntityName);
		SetMoveSpeed(1000);
		NomalSkill = EntityData.NormalSkill;
		SpecialSkill = EntityData.SpecialSkill;
		UE_LOG(LogTemp,Error,TEXT("!Entity Name: %s, HP: %d, Move Speed: %d"),
			*EntityData.EntityName,EntityData.HP,EntityData.MoveSpeed);

	} else{
		//		UE_LOG(LogTemp,Error,TEXT("Loadgame EntityData 찾기 실패: %s"),*currentPreset);
	}
	// currentPreset!=클릭한 버튼의 캐릭터이름 이면..
	if(pastPreset != currentPreset) {
		PlayerStatComponent->TransformToEntity(EntityData.EntityGroupID,EntityData.HP,EntityData.TransManaCost);
		pastPreset = currentPreset;
		//	UE_LOG(LogTemp,Error,TEXT("ChangePreset = pastPreset %s != currentPreset %s"),*pastPreset,currentPreset);
	}

	USkeletalMeshComponent* MeshComponent = GetMesh();
}

void AN_Graduation_projectCharacter::SetMoveSpeed(int32 MoveSpeed)
{
	//UE_LOG(LogTemp,Error,TEXT("!currentSpeed: %d"),currentSpeed);

	currentSpeed = MoveSpeed;
	GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	if(currentSpeed == 0)
	{
		GetCharacterMovement()->StopMovementImmediately();
	}
}
void AN_Graduation_projectCharacter::StartAction()
{
	UE_LOG(LogTemp,Error,TEXT("StartAction"));
	bCanMove = false;
	// 플레이어 컨트롤러 가져오기
	//APlayerController* PlayerController = Cast<APlayerController>(GetController());

	//if(PlayerController)
	//{
	//	if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	//	{
	//		Subsystem->RemoveMappingContext(DefaultMappingContext);
	//		UE_LOG(LogTemp,Error,TEXT("Input Mapping Context Removed"));
	//	} 
	//}

}
void AN_Graduation_projectCharacter::EndAction()
{
	UCharacterStateComponent* StateComp = FindComponentByClass<UCharacterStateComponent>();
	StateComp->ChangeState(ECharacterState::Idle);
	CharacterStateComponent->isAction = false;
	bCanMove=true;
	// 기존 속도로 이동 
	UE_LOG(LogTemp,Error,TEXT("EndAction"));
	//	// Action 상태 종료 시 입력 활성화
	//	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	//	if(PlayerController)
	//	{
	//		if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	//		{
	//			Subsystem->AddMappingContext(DefaultMappingContext,0);
	//			UE_LOG(LogTemp,Error,TEXT("Input Mapping Context Restored"));
	//		}
	//
	//		UE_LOG(LogTemp,Error,TEXT("Yes Action PlayerController!"));
	//	}
}
void AN_Graduation_projectCharacter::OnPlayerDead()
{
	UCharacterStateComponent* StateComp = FindComponentByClass<UCharacterStateComponent>();
	if(StateComp)
	{
		FString StateString = UEnum::GetValueAsString(StateComp->CurrentState);
		StateComp->ChangeState(ECharacterState::Dead);
	}
	// HP가 0이 되었을 때 처리할 로직
	UE_LOG(LogTemp,Warning,TEXT("Player is dead"));
	isDead=true;
	bcanPie=false;
	auto* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if(!MyGameInstance) return;
	MyGameInstance->LoadGame();
	WidgetActor->ShowDieWidget();
}

void AN_Graduation_projectCharacter::SetPreset(FString PresetReference)
{
	TArray<UBoxComponent*> Components;
	GetComponents<UBoxComponent>(Components);

	for(UBoxComponent* Comp : Components)
	{
		if(Comp && Comp->GetName().Contains(TEXT("Hitbox2")))
		{
			Comp->DestroyComponent();
		}
	}
	UE_LOG(LogTemp,Log,TEXT("ChangePreset SetPreset 실행됨"));
	USkeletalMeshComponent* MeshComponent = GetMesh();
	currentPreset  = PresetReference.TrimStartAndEnd(); //공백 제거

	//	currentPreset = PresetReference;
	InvincibleOriginalMaterial = nullptr;	// 프리셋 이름마다 메시 에셋 파일 할당
	if(currentPreset == "PCPreset.uasset")
	{
		//<변신을 위해 이거 2개 필수>
		FSoftObjectPath MeshPath(TEXT("/Game/Gamin/Player/Player_Attack/Player_Attack_UVW.Player_Attack_UVW"));
		TSubclassOf<UAnimInstance> NewAnimBP = LoadClass<UAnimInstance>(nullptr,TEXT("/Game/Characters/MyGameCharacter/MyPlayerAnimBlueprint.MyPlayerAnimBlueprint_C"));

		//<피격을 위해 이거 3개 필수>
		UMaterialInterface* PlayerMaterial = LoadObject<UMaterialInterface>(nullptr,TEXT("MaterialInterface'/Game/Gamin/Player/Player_Attack/phong2.phong2'"));
		InvincibleOriginalMaterial = PlayerMaterial;
		MeshComponent->SetMaterial(0,InvincibleOriginalMaterial);

		USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshPath.TryLoad());
		GetMesh()->SetSkeletalMesh(LoadedMesh);
		//PlayerSword->SetHiddenInGame(true);  
		if(LoadedMesh)
		{
			GetMesh()->SetRelativeScale3D(FVector(1.0f,1.0f,1.0f));
			PlayerSword->SetHiddenInGame(false);

			// 스켈레탈 메시를 사용할 경우 SetSkeletalMesh() 사용
			GetMesh()->SetSkeletalMesh(LoadedMesh);
			GetMesh()->SetAnimInstanceClass(NewAnimBP);

			SpawnHitBoxAtSocket("AttachHitBox");
			TrySpawnHitBox("AttachHitBox2");

		}
	}

	else if(currentPreset == "WildBoarPreset.uasset")
	{
		//<변신을 위해 이거 2개 필수>
		FSoftObjectPath MeshPath(TEXT("/Game/Gamin/Bore_UVW/Bore_attack_uvw_2.Bore_attack_uvw_2"));
		TSubclassOf<UAnimInstance> NewAnimBP = LoadClass<UAnimInstance>(nullptr,TEXT("/Game/Characters/MyGameCharacter/MyWildBoar_Skeleton_AnimBP.MyWildBoar_Skeleton_AnimBP_C"));

		//<피격을 위해 이거 3개 필수>
		UMaterialInterface* BoarMaterial = LoadObject<UMaterialInterface>(nullptr,TEXT("MaterialInterface'/Game/Gamin/Bore_UVW/lambert2.lambert2'"));
		InvincibleOriginalMaterial = BoarMaterial;
		MeshComponent->SetMaterial(0,InvincibleOriginalMaterial);

		USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshPath.TryLoad());
		if(LoadedMesh && NewAnimBP)
		{
			GetMesh()->SetRelativeScale3D(FVector(2.0f,2.0f,2.0f));
			GetMesh()->SetSkeletalMesh(LoadedMesh);
			GetMesh()->SetAnimInstanceClass(NewAnimBP);

			SpawnHitBoxAtSocket("AttachHitBox");
			TrySpawnHitBox("AttachHitBox2");

		}
	}

	else if(currentPreset == "InpermonPreset.uasset")
	{
		//<변신을 위해 이거 2개 필수>
		FSoftObjectPath MeshPath(TEXT("/Game/Gamin/InferMon/InferMon_UVW.InferMon_UVW"));
		TSubclassOf<UAnimInstance> NewAnimBP = LoadClass<UAnimInstance>(nullptr,TEXT("/Game/Gamin/InferMon/Infermon_Skeleton_AnimBP.Infermon_Skeleton_AnimBP_C"));

		//<피격을 위해 이거 3개 필수>
		UMaterialInterface* Mat0 = LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Gamin/InferMon/InferMon_UVW_solid_Mat.InferMon_UVW_solid_Mat"));
		UMaterialInterface* Mat1 = LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Gamin/InferMon/InferMon_tail_UVW_solid_Mat.InferMon_tail_UVW_solid_Mat"));
		UMaterialInterface* Mat2 = LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Gamin/InferMon/InferMon_body_UVW_solid_Mat.InferMon_body_UVW_solid_Mat"));
		if(Mat0 && Mat1 && Mat2)
		{
			MeshComponent->SetMaterial(0,Mat0);
			MeshComponent->SetMaterial(1,Mat1);
			MeshComponent->SetMaterial(2,Mat2);
		}
		//UMaterialInterface* InpermonMaterial = LoadObject<UMaterialInterface>(nullptr,TEXT("MaterialInterface'/Game/Gamin/InferMon/standardSurface1.standardSurface1'"));
		//InvincibleOriginalMaterial = InpermonMaterial;
		//MeshComponent->SetMaterial(0,InvincibleOriginalMaterial);

		USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshPath.TryLoad());
		if(LoadedMesh && NewAnimBP)
		{
			GetMesh()->SetRelativeScale3D(FVector(3.f,3.0f,3.0f));
			GetMesh()->SetSkeletalMesh(LoadedMesh);
			GetMesh()->SetAnimInstanceClass(NewAnimBP);
		}
	}

	else if(currentPreset == "FreezardPreset.uasset")
	{
		FSoftObjectPath MeshPath(TEXT("/Game/Gamin/Freezard/Freezard_idle.Freezard_idle"));
		TSubclassOf<UAnimInstance> NewAnimBP = LoadClass<UAnimInstance>(nullptr,TEXT("/Game/Gamin/Freezard/Freezard_Skeleton_AnimBP.Freezard_Skeleton_AnimBP_C"));

		//<피격을 위해 이거 3개 필수>
		UMaterialInterface* FreezardMaterial = LoadObject<UMaterialInterface>(nullptr,TEXT("MaterialInterface'/Game/Gamin/Freezard/FreeZard_solid_Mat.FreeZard_solid_Mat'"));
		InvincibleOriginalMaterial = FreezardMaterial;
		MeshComponent->SetMaterial(0,InvincibleOriginalMaterial);

		USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshPath.TryLoad());
		if(LoadedMesh)
		{
			GetMesh()->SetRelativeScale3D(FVector(4.0f,4.0f,4.0f));
			GetMesh()->SetSkeletalMesh(LoadedMesh);
			GetMesh()->SetAnimInstanceClass(NewAnimBP);

			SpawnHitBoxAtSocket("AttachHitBox");
			TrySpawnHitBox("AttachHitBox2");

		}
	}

	else if(currentPreset == "StoneGolemPreset.uasset")
	{
		FSoftObjectPath MeshPath(TEXT("/Game/Gamin/StoneGolem/StonGolem_walk.StonGolem_walk"));
		TSubclassOf<UAnimInstance> NewAnimBP = LoadClass<UAnimInstance>(nullptr,TEXT("/Game/Gamin/StoneGolem/StonGolem_Skeleton_AnimBP.StonGolem_Skeleton_AnimBP_C"));

		//<피격을 위해 이거 3개 필수>
		UMaterialInterface* GolemMaterial = LoadObject<UMaterialInterface>(nullptr,TEXT("MaterialInterface'/Game/Asset/SyntyAsset/PolygonFantasyRivals/Materials/M_FantasyRivals_01_A.M_FantasyRivals_01_A'"));
		InvincibleOriginalMaterial = GolemMaterial;
		MeshComponent->SetMaterial(0,InvincibleOriginalMaterial);

		USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshPath.TryLoad());
		if(LoadedMesh)
		{
			GetMesh()->SetRelativeScale3D(FVector(2.0f,2.0f,2.0f));
			m_pMeshCom->SetSkeletalMesh(LoadedMesh);
			GetMesh()->SetAnimInstanceClass(NewAnimBP);

			SpawnHitBoxAtSocket("ArmSocket2");
			TrySpawnHitBox("AttachHitBox2");
			SpawnHitSphereAtSocket("BreakerSocket2");
		}
	} else if(currentPreset == "SkeletonWarriorPreset.uasset")
	{
		FSoftObjectPath MeshPath(TEXT("/Game/Gamin/Skeleton_Warrior/SkeletonWarrior_Shield_Guard.SkeletonWarrior_Shield_Guard"));
		//FSoftObjectPath MeshPath(TEXT("/Game/Gamin/Skeleton_Warrior/SkeletonWarrior_idle_Skeleton.SkeletonWarrior_idle_Skeleton"));

		TSubclassOf<UAnimInstance> NewAnimBP = LoadClass<UAnimInstance>(nullptr,TEXT("/Game/Gamin/Skeleton_Warrior/SkeletonWarrior_Skeleton_AnimBP.SkeletonWarrior_Skeleton_AnimBP_C"));

		//<피격을 위해 이거 3개 필수>
		UMaterialInterface* WarriorMaterial = LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Animation/Skeleton/M_PolygonDarkFantasy_01_A.M_PolygonDarkFantasy_01_A"));
		InvincibleOriginalMaterial = WarriorMaterial;
		MeshComponent->SetMaterial(0,InvincibleOriginalMaterial);

		USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshPath.TryLoad());
		if(LoadedMesh)
		{
			SkeletonShield ->SetHiddenInGame(false);
			SkeletonSword->SetHiddenInGame(false);
			GetMesh()->SetRelativeScale3D(FVector(1.0f,1.0f,1.0f));
			m_pMeshCom->SetSkeletalMesh(LoadedMesh);
			GetMesh()->SetAnimInstanceClass(NewAnimBP);

		//	SpawnHitBoxAtSocket("AttachHitBox");
		//	TrySpawnHitBox("AttachHitBox2");

		}
	} else if(currentPreset == "SkeletonArcherPreset.uasset")
	{
		FSoftObjectPath MeshPath(TEXT("/Game/Gamin/Skeleton_Archer/SkeletonArcher_attack1.SkeletonArcher_attack1"));
		TSubclassOf<UAnimInstance> NewAnimBP = LoadClass<UAnimInstance>(nullptr,TEXT("/Game/Gamin/Skeleton_Archer/SkeletonArcher_Skeleton_AnimBP.SkeletonArcher_Skeleton_AnimBP_C"));
		//D:/GitHub/N-Graduation-project/N_Graduation_project/Content/Animation/Skeleton/M_PolygonDarkFantasy_01_A.uasset
		//<피격을 위해 이거 3개 필수>
		UMaterialInterface* ArcherMaterial =  LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Animation/Skeleton/M_PolygonDarkFantasy_01_A.M_PolygonDarkFantasy_01_A"));
		InvincibleOriginalMaterial = ArcherMaterial;
		MeshComponent->SetMaterial(0,InvincibleOriginalMaterial);

		USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshPath.TryLoad());
		if(LoadedMesh)
		{
			GetMesh()->SetRelativeScale3D(FVector(1.0f,1.0f,1.0f));
			m_pMeshCom->SetSkeletalMesh(LoadedMesh);
			GetMesh()->SetAnimInstanceClass(NewAnimBP);
			SkeletonBow ->SetHiddenInGame(false);
			//SpawnHitBoxAtSocket("AttachHitBox");
			//TrySpawnHitBox("AttachHitBox2");

		}
	}

}
//히트박스 동적 생성
void AN_Graduation_projectCharacter::SpawnHitBoxAtSocket(FName SocketName)
{
	UBoxComponent* HitBox = nullptr;

	// 1. 이미 존재하는 히트박스를 찾기
	TArray<UBoxComponent*> Components;
	GetComponents<UBoxComponent>(Components);

	for(UBoxComponent* Comp : Components)
	{
		if(Comp && Comp->GetName().Contains(TEXT("Hitbox")))
		{
			HitBox = Comp;
			break;
		}
	}

	if(!HitBox)
	{
		HitBox = NewObject<UBoxComponent>(this);
		// 이름 설정
		HitBox->Rename(TEXT("Hitbox"));

		// RegisterComponent()로 엔진에 등록 (꼭 넣어야 함)
		HitBox->RegisterComponent();
		HitBox->OnComponentBeginOverlap.AddDynamic(this,&AN_Graduation_projectCharacter::OnHitboxOverlap);
		// 소켓에 붙이기
		HitBox->AttachToComponent(GetMesh(),FAttachmentTransformRules::SnapToTargetIncludingScale,SocketName);
		//크기 설정
		HitBox->SetBoxExtent(FVector(30.f,20.f,10.f));
		// 충돌 설정 (QueryOnly-> 물리충돌 무시하고 오버랩만 처리)
		HitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		//충돌 채널
		HitBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
		//모든 충돌 무시
		HitBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		HitBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECollisionResponse::ECR_Overlap);
		HitBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic,ECollisionResponse::ECR_Overlap);
		HitBox->SetGenerateOverlapEvents(true);
		HitBox->ComponentTags.Add(FName("HitBox"));

		HitBox->SetHiddenInGame(true); // 히트박스 안보이게-> true

		PlayerSkillComponent->SetHitBox(HitBox);
		//UE_LOG(LogTemp,Warning,TEXT("Playerpapago hHitBox Address: %p"),HitBox);

		PlayerSkillComponent->HideHitBox();
	}

	HitBox->AttachToComponent(GetMesh(),FAttachmentTransformRules::SnapToTargetIncludingScale,SocketName);
	PlayerSkillComponent->SetHitBox(HitBox);

	//UE_LOG(LogTemp,Warning,TEXT("HitBox reused and attached to socket: %s"),*SocketName.ToString());
}
void AN_Graduation_projectCharacter::TrySpawnHitBox(FName SocketName)
{
	if(!GetMesh()) return;

	if(GetMesh()->DoesSocketExist(SocketName))
	{
		SpawnHitBoxAtSocket2(SocketName);
	} else
	{
		//UE_LOG(LogTemp,Warning,TEXT("소켓 %s 이(가) 존재하지 않아 히트박스를 생성하지 않음"),*SocketName.ToString());
	}
}

void AN_Graduation_projectCharacter::SpawnHitBoxAtSocket2(FName SocketName)
{
	UBoxComponent* HitBox2 = nullptr;

	// 1. 이미 존재하는 히트박스를 찾기
	TArray<UBoxComponent*> Components;
	GetComponents<UBoxComponent>(Components);

	for(UBoxComponent* Comp : Components)
	{
		if(Comp && Comp->GetName().Contains(TEXT("Hitbox2")))
		{
			HitBox2 = Comp;
			break;
		}
	}

	if(!HitBox2)
	{
		HitBox2 = NewObject<UBoxComponent>(this);

		// 이름 설정
//		HitBox2->Rename(TEXT("Hitbox2"));
		HitBox2 = NewObject<UBoxComponent>(this,UBoxComponent::StaticClass(),TEXT("Hitbox2"));

		// RegisterComponent()로 엔진에 등록 (꼭 넣어야 함)
		HitBox2->RegisterComponent();
		HitBox2->OnComponentBeginOverlap.AddDynamic(this,&AN_Graduation_projectCharacter::OnHitboxOverlap);
		// 소켓에 붙이기
		HitBox2->AttachToComponent(GetMesh(),FAttachmentTransformRules::SnapToTargetIncludingScale,SocketName);
		//크기 설정
		HitBox2->SetBoxExtent(FVector(30.f,20.f,10.f));
		// 충돌 설정 (QueryOnly-> 물리충돌 무시하고 오버랩만 처리)
		HitBox2->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		//충돌 채널
		HitBox2->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
		//모든 충돌 무시
		HitBox2->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		HitBox2->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECollisionResponse::ECR_Overlap);
		HitBox2->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic,ECollisionResponse::ECR_Overlap);
		HitBox2->SetGenerateOverlapEvents(true);
		HitBox2->ComponentTags.Add(FName("HitBox"));

		HitBox2->SetHiddenInGame(true); // 히트박스 안보이게-> true

		PlayerSkillComponent->SetHitBox2(HitBox2);
		//UE_LOG(LogTemp,Warning,TEXT("papago hHitBox Address: %p"),HitBox2);

		PlayerSkillComponent->HideHitBox2();
	}
	HitBox2->AttachToComponent(GetMesh(),FAttachmentTransformRules::SnapToTargetIncludingScale,SocketName);
	PlayerSkillComponent->SetHitBox(HitBox2);

}
void AN_Graduation_projectCharacter::SpawnHitSphereAtSocket(FName SocketName)
{
	if(!GetMesh()->DoesSocketExist(SocketName))
	{
		UE_LOG(LogTemp,Warning,TEXT("Socket '%s' does not exist on the mesh."),*SocketName.ToString());
		return;
	}

	USphereComponent* HitSphere = nullptr;

	// 이미 존재하는 히트스피어 찾기
	TArray<USphereComponent*> Components;
	GetComponents<USphereComponent>(Components);

	for(USphereComponent* Comp : Components)
	{
		if(Comp && Comp->GetName().Contains(TEXT("HitSphere")))
		{
			HitSphere = Comp;
			break;
		}
	}

	if(!HitSphere)
	{
		HitSphere = NewObject<USphereComponent>(this);
		HitSphere->Rename(TEXT("HitSphere"));

		HitSphere->RegisterComponent();
		HitSphere->OnComponentBeginOverlap.AddDynamic(this,&AN_Graduation_projectCharacter::OnHitboxOverlap);

		// 크기 설정
		HitSphere->SetSphereRadius(40.f); // 원하는 크기로 조절

		// 충돌 설정
		HitSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		HitSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
		HitSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		HitSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECollisionResponse::ECR_Overlap);
		HitSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic,ECollisionResponse::ECR_Overlap);
		HitSphere->SetGenerateOverlapEvents(true);

		HitSphere->ComponentTags.Add(FName("HitSphere"));
		HitSphere->SetHiddenInGame(true);
		HitSphere->SetRelativeLocation(FVector(30.f,30.f,50.f));
		PlayerSkillComponent->SetHitSphere(HitSphere);
	}

	HitSphere->AttachToComponent(GetMesh(),FAttachmentTransformRules::SnapToTargetIncludingScale,SocketName);
	PlayerSkillComponent->SetHitSphere(HitSphere);

	//UE_LOG(LogTemp, Warning, TEXT("HitSphere attached to socket: %s"), *SocketName.ToString());
}

void AN_Graduation_projectCharacter::OnHitboxOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult)
{
	//	UE_LOG(LogTemp,Warning,TEXT("OnHitboxOverlap된 객체: %s"),*OtherActor->GetName());
	for(const FName& Tag : OtherActor->Tags)
	{
		//UE_LOG(LogTemp,Warning,TEXT("OtherActor Tag: %s"),*Tag.ToString());
	}

	AActor* MyCharacter = Cast<AActor>(UGameplayStatics::GetPlayerCharacter(this,0));
	if(PlayerSkillComponent->CanUseNomalSkill || PlayerSkillComponent->CanUseSpecialSkill)
	{
		if(OtherActor != MyCharacter)
		{
			if(!PlayerSkillComponent->DamagedActors.Contains(OtherActor)) //데미지를 받은 적 있는지 확인 후
			{
				if(OtherActor->ActorHasTag(FName("NK_Object"))){
					PlayerSkillComponent->DamagedActors.Add(OtherActor);
					//	UE_LOG(LogTemp,Warning,TEXT("DamagedActors에 추가된 NK_Object: %s"),*OtherActor->GetName());
				}

				float Damage = PlayerSkillComponent->DamageAmount;
				//UGameplayStatics::ApplyDamage(OtherActor, Damage, GetController(), this, nullptr); //데미지를 줌
				if(PlayerSkillComponent->DamagedActors.Contains(OtherActor) == false)
				{
					PlayerSkillComponent->DamagedActors.Add(OtherActor);//중복 없이 데미지 받은 객체저장
					UE_LOG(LogTemp,Warning,TEXT("OnHitboxOverlap DamagedActors에 추가된 액터: %s"),*OtherActor->GetName());
					//PlayerSkillComponent->SnapshotDamagedActors = PlayerSkillComponent->DamagedActors.Array();
				}
			}
		}

	}

}
//변신
void AN_Graduation_projectCharacter::ChangePreset(FString Name)
{

	currentPreset = Name;
	if(currentPreset  != pastPreset) {

		if(PlayerSword)
		{
			SkeletonBow ->SetHiddenInGame(true);

			PlayerSword->SetHiddenInGame(true);
			SkeletonShield ->SetHiddenInGame(true);
			SkeletonSword->SetHiddenInGame(true);
		} else
		{
			//UE_LOG(LogTemp,Error,TEXT("PlayerSword is nullptr in ChangePreset"));
		}		UpdateEntityData();
		if(OkTrans) {
			//UE_LOG(LogTemp,Warning,TEXT("ChangePreset 변신완 "));

			SetPreset(EntityData.PresetReference);
			WidgetActor->Back_CacheFinalMouseAngle = false;
			//UE_LOG(LogTemp,Warning,TEXT("OkTrans true"));
		} else {
			WidgetActor->Back_CacheFinalMouseAngle = true;
		}
	}

}
//void AN_Graduation_projectCharacter:: LoadPreset(FString PresetID){
//	if(PlayerSword)
//	{
//		PlayerSword->SetHiddenInGame(true);
//	} else
//	{
//		UE_LOG(LogTemp,Error,TEXT("PlayerSword is nullptr in ChangePreset"));
//	}
//	FString CleanName;
//	auto* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
//	if(!MyGameInstance) return;
//	// 확장자 제거
//	if(MyGameInstance->CurrentPlayerCharacter!="PCPreset.uasset"){ 
//	FString FileName = FPaths::GetBaseFilename(MyGameInstance->CurrentPlayerCharacter);
//	CleanName = FileName;
//	CleanName.RemoveFromEnd(TEXT("Preset"));}
//	else{
//		CleanName="PlayerCharacter";
//	}
//	if(UABGameSingleton::Get().GetEntityDataByGroupID(CleanName,EntityData))
//	{
//		if(PlayerStatComponent->CurrentMana >= EntityData.TransManaCost) 		OkTrans = true;
//		if(!(PlayerStatComponent->CurrentMana >= EntityData.TransManaCost))		OkTrans = false;
//
//		InvincibleOriginalMaterial = nullptr;
//		//SetActorLabel(EntityData.EntityName);
//		SetMoveSpeed(1000);
//		NomalSkill = EntityData.NormalSkill;
//		SpecialSkill = EntityData.SpecialSkill;
//		UE_LOG(LogTemp,Error,TEXT("Loadgame LoadPreset Entity Name: %s, HP: %d, Move Speed: %d"),
//			*EntityData.EntityName,EntityData.HP,EntityData.MoveSpeed);
//		UE_LOG(LogTemp,Error,TEXT("Loadgame LoadPreset EntityData 찾기 성공: %s"),*CleanName);
//
//	} else{
//			UE_LOG(LogTemp,Error,TEXT("Loadgame LoadPreset EntityData 찾기 실패: %s"),*CleanName);
//	}
//
//	UE_LOG(LogTemp,Error,TEXT("Loadgame LoadPreset 실행됨"));
//	PlayerStatComponent->TransformToEntity(EntityData.EntityGroupID,EntityData.HP,EntityData.TransManaCost);
//	pastPreset = MyGameInstance->CurrentPlayerCharacter;
//	SetPreset(EntityData.PresetReference);
//	WidgetActor->Back_CacheFinalMouseAngle = false;
//
//
//	USkeletalMeshComponent* MeshComponent = GetMesh();
//}