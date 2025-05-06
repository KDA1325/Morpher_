#include "PlayerSkillComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "MyPlayerStatComponent.h"
#include "WidgetActor.h"
#include "TimerManager.h"
#include "CharacterStateComponent.h" //state
#include "EngineUtils.h"
#include "N_Graduation_project/N_Graduation_projectCharacter.h"
#include "EntitySkillComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"            // UDecalComponent
#include "Kismet/KismetMathLibrary.h"             // UKismetMathLibrary::FindLookAtRotation
#include "PlayerProjectile.h"

UPlayerSkillComponent::UPlayerSkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	IsDefending = false;
	OnceHitBox = false;
	CanUseNomalSkill = true;
	CanUseSpecialSkill = true;
	DamageAmount = 50;

	static ConstructorHelpers::FClassFinder<APlayerProjectile> ProjectileBP(TEXT("/Game/Entity/BP/BP_Player_Inpermon_Projectile_ThrowRock"));
	if(ProjectileBP.Succeeded())
	{
		NomalProjectileClass = ProjectileBP.Class;
	}
}

void UPlayerSkillComponent::BeginPlay()
{
	Super::BeginPlay();
	// UMaterialInstance를 로드
	ChargeDecalMaterial = LoadObject<UMaterialInstance>(nullptr,TEXT("MaterialInstance'/Game/Entity/M_WildBoarChargeDecal1.M_WildBoarChargeDecal1'"));
	ChargeDecalComponent = NewObject<UDecalComponent>(this);
	if(ChargeDecalComponent)
	{
		ChargeDecalComponent->RegisterComponent();  // 컴포넌트 등록
		ChargeDecalComponent->AttachToComponent(Cast<AActor>(GetOwner())->GetRootComponent(),FAttachmentTransformRules::KeepWorldTransform);
		ChargeDecalComponent->SetDecalMaterial(ChargeDecalMaterial);  // 데칼 머티리얼 설정
	} else
	{
		UE_LOG(LogTemp,Warning,TEXT("Failed to create ChargeDecalComponent"));
	}
}
void UPlayerSkillComponent::SetHitBox(UBoxComponent* InHitBox)
{
	PlayerHitBox = InHitBox;
	UE_LOG(LogTemp, Warning, TEXT("papago PlayerHitBox Address: %p"), PlayerHitBox);

}
/* 스킬 관련 */
void UPlayerSkillComponent::OnDefenseSkill()
{
	IsDefending = true;
	if (GEngine)
	{
		//	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("DefenseSkill on"));
	}
}

void UPlayerSkillComponent::OffDefenseSkill()
{
	IsDefending = false;
}

void UPlayerSkillComponent::SetSkillTimer(float Count, FTimerDelegate End)
{
	if (GetWorld() && Count > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(NomalSkillTimerHandle, End, Count, false);
		UE_LOG(LogTemp, Log, TEXT("hum SetSkillTimer사용됨"));
	}
}

void UPlayerSkillComponent::SpecialSetSkillTimer(float Count, FTimerDelegate End)
{
	if (GetWorld() && Count > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(SpecialSkillTimerHandle, End, Count, false);
		UE_LOG(LogTemp, Log, TEXT("OnMontag SpecialSetSkillTimer사용됨"));
	}
}
void UPlayerSkillComponent::ChargeSkillTimer(float Delay, FTimerDelegate End)
{
	if (GetWorld() && Delay > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(ChargeSkillTimerHandle, End, Delay, false);
		UE_LOG(LogTemp, Log, TEXT("ChargeSkillTimer 사용됨"));
	}
}

//쿨타임
void UPlayerSkillComponent::NomalCooldown()
{
	CanUseNomalSkill = true;

	UE_LOG(LogTemp, Log, TEXT("Kakao NomalCooldown"));
}
void UPlayerSkillComponent::SpecialCooldown()
{
	CanUseSpecialSkill = true;
}

//히트박스
void UPlayerSkillComponent::SettingHitBox(const FSkillData& SkillData)
{
	if (!OnceHitBox && PlayerHitBox)
	{
		// 히트박스 크기 및 위치 설정
		FVector NewBoxExtent = FVector(SkillData.SkillTypeSizeX, SkillData.SkillTypeSizeY, 100);
		PlayerHitBox->SetBoxExtent(NewBoxExtent);

		FVector NewLocation = PlayerHitBox->GetRelativeLocation();
		NewLocation.X += SkillData.SkillTypeSizeX;
		PlayerHitBox->SetRelativeLocation(NewLocation);

		OnceHitBox = true;
	}
}
void UPlayerSkillComponent::OnHitBox(const FSkillData& SkillData)
{
	if (PlayerHitBox)
	{
		PlayerHitBox->SetVisibility(true);
		PlayerHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);  // 충돌 키기

		UE_LOG(LogTemp, Warning, TEXT("kakao On OnHitBox"));
		UE_LOG(LogTemp, Warning, TEXT("papago On PlayerHitBox  Address: %p"), PlayerHitBox);

	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("gugugu Failed to find HitBox "));
		UE_LOG(LogTemp, Warning, TEXT("papago On Failed PlayerHitBox Address: %p"), PlayerHitBox);

	}
}
void UPlayerSkillComponent::HideHitBox()
{
	PlayerHitBox->SetVisibility(false);  // 자식까지 숨기기
	PlayerHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);  // 충돌 꺼버리기
	UE_LOG(LogTemp, Warning, TEXT("HitBox and Arrow hidden."));
	UE_LOG(LogTemp, Warning, TEXT("papago hidden PlayerHitBox  Address: %p"), PlayerHitBox);

	// 데미지 체크 초기화
	DamagedActors.Empty();
	SnapshotDamagedActors.Empty();
}

AActor* UPlayerSkillComponent::FindFrontMonsterTarget() const
{
	if (!GetWorld()) return nullptr;

	AActor* ClosestMonster = nullptr;
	float MinDistance = FLT_MAX;

	AActor* Owner = GetOwner();
	if (!Owner) return nullptr;

	FVector PlayerLocation = Owner->GetActorLocation();

	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (Actor && Actor->ActorHasTag(FName("Monster")))
		{
			float CurrentDistance = FVector::Dist(PlayerLocation, Actor->GetActorLocation());

			if (CurrentDistance < MinDistance)
			{
				MinDistance = CurrentDistance;
				ClosestMonster = Actor;
			}
		}
	}

	return ClosestMonster;
}

float UPlayerSkillComponent::GetDistanceTo(const AActor* OtherActor) const
{
	// 현재 액터와 플레이어 간 거리 계산
	return OtherActor ? (GetOwner()->GetActorLocation() - OtherActor->GetActorLocation()).Size() : 0.f;
}

float UPlayerSkillComponent::MeasureDistanceToMonster() const
{
	AActor* MonsterTarget = FindFrontMonsterTarget();
	if (MonsterTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetDistanceTo(MonsterTarget): %f"), GetDistanceTo(MonsterTarget));
		return GetDistanceTo(MonsterTarget);
	}
	return 0.f;
}

/* 스킬 실행 */
void UPlayerSkillComponent::VisibleShapeBox(const FString& SkillID)
{
	FSkillData SkillData;
	if (!UABGameSingleton::Get().GetSkillDataBySkillID(SkillID, SkillData)) return;
	if (SkillData.SkillType == EnumSkillType::HitBox)
	{
		if (SkillData.SkillTypeShape == EnumSkillTypeShape::Box)
		{
			SettingHitBox(SkillData);  // 히트박스 초기화
			OnHitBox(SkillData);    // 히트박스 활성화
		}
		else if (SkillData.SkillTypeShape == EnumSkillTypeShape::Sphere)
		{
			//	 Sphere 관련 처리 추가
		}
	}
	else if (SkillData.SkillType == EnumSkillType::Projectile) 
	{
		UE_LOG(LogTemp,Warning,TEXT("Projectile 실행됨"));
	
	}
	else 
	{
		//OnDefenseSkill();
	}
}
void UPlayerSkillComponent::NomalSkillPlay(const FString& SkillID)
{
	distance = MeasureDistanceToMonster();

	if (CanUseNomalSkill)
	{
		FSkillData SkillData;
		if (!UABGameSingleton::Get().GetSkillDataBySkillID(SkillID, SkillData)) return;

		AN_Graduation_projectCharacter* MyChar = GetOwner<AN_Graduation_projectCharacter>();
		MyChar->StartAction();


		//if (distance > 0 && distance <= SkillData.SkillRange)

		UE_LOG(LogTemp, Warning, TEXT("KKakao Distance to Monster: %f"), distance);

		UE_LOG(LogTemp, Warning, TEXT("%f Kkakao Yes distance"), SkillData.SkillRange);
		VisibleShapeBox(SkillID); // 적중 범위 표시
		SkillEffect(SkillID);

		// 스킬 쿨타임 시작
		CanUseNomalSkill = false;

		auto StatComponent = GetOwner()->FindComponentByClass<UWidgetActor>();
		if (StatComponent && StatComponent->HUDWidget)
		{
			StatComponent->HUDWidget->UpdateNomalSkillCooldown(SkillData.SkillCoolTime, CanUseNomalSkill, CanUseSpecialSkill);
			StatComponent->HUDWidget->CanNomal = false;
		}

		// 스킬 애니메이션
		SkillAnimation(SkillID);
		UE_LOG(LogTemp, Warning, TEXT("amam OnMontag Playing %s"), *SkillID);


		// 쿨타임 타이머 설정
		FTimerDelegate NomalCooldownEnd;
		NomalCooldownEnd.BindUObject(this, &UPlayerSkillComponent::NomalCooldown);
		SetSkillTimer(SkillData.SkillCoolTime, NomalCooldownEnd);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("kakao No CanUseNomalSkill"));
	}
}

void UPlayerSkillComponent::SpecialSkillPlay(const FString& SkillID)
{
	//UE_LOG(LogTemp, Warning, TEXT("kakao On SpecialSkillPlay"));
	distance = MeasureDistanceToMonster();

	if (CanUseSpecialSkill)
	{
		FSkillData SkillData;
		if (!UABGameSingleton::Get().GetSkillDataBySkillID(SkillID, SkillData)) return;

		AN_Graduation_projectCharacter* MyChar = GetOwner<AN_Graduation_projectCharacter>();
		MyChar->StartAction();

		if (SkillID == "Skill_Charge")
		{
			StoredDashDirection = MyChar->GetActorForwardVector().GetSafeNormal();

			DrawChargePath(); // 돌진 선

			float PrepTime = 1.0f;

			FTimerDelegate ChargeEnd;
			ChargeEnd.BindUFunction(this, FName("ExecuteChargeDash"), StoredDashDirection, SkillID);
			ChargeSkillTimer(PrepTime, ChargeEnd);
			// 범위 내일 때만 히트 판정 박스 표시
		}
		if (SkillID == "Skill_ShieldGuard") {
			//VisibleShapeBox(SkillID);
			OnDefenseSkill();
		}
		CanUseSpecialSkill = false;

		auto StatComponent = GetOwner()->FindComponentByClass<UWidgetActor>();
		if (StatComponent && StatComponent->HUDWidget)
		{
			StatComponent->HUDWidget->UpdateSpecialSkillCooldown(SkillData.SkillCoolTime, CanUseNomalSkill, CanUseSpecialSkill);
			StatComponent->HUDWidget->CanSpecial = false;
		}

		// 스킬 애니메이션
		SkillAnimation(SkillID);
		UE_LOG(LogTemp, Warning, TEXT("OnMontag Playing %s"), *SkillID);

		FTimerDelegate SpecialCooldownEnd;
		SpecialCooldownEnd.BindUObject(this, &UPlayerSkillComponent::SpecialCooldown);
		SpecialSetSkillTimer(SkillData.SkillCoolTime, SpecialCooldownEnd);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("kakao No CanUseSpecialSkill"));
	}
}
//<돌진>
void UPlayerSkillComponent::DrawChargePath()
{
	ACharacter* MyChar = Cast<ACharacter>(GetOwner());
	if(!MyChar || !GetWorld()) return;

	FVector StartLocation = MyChar->GetActorLocation();
	//float DashSpeed = 2000.0f;
	//float DashDuration = 0.75f;
	//FVector EndLocation = StartLocation + MyChar->GetActorForwardVector() * DashSpeed * DashDuration;
	FVector EndLocation = StartLocation + MyChar->GetActorForwardVector() * 600.f;

	//DrawDebugLine(GetWorld(),StartLocation,EndLocation,FColor::Red,false,1.5f,0,5.f);
	SpawnChargeIndicator(StartLocation,EndLocation);  // 디버그 라인 대신 데칼 사용

	// 1초 후 자동 제거
	FTimerHandle DecalTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DecalTimerHandle,[this]()
	{
		if(ChargeDecalComponent)
		{
			ChargeDecalComponent->DestroyComponent();
			ChargeDecalComponent = nullptr;
		}
	},1.0f,false);

	UE_LOG(LogTemp,Warning,TEXT("DrawChargePath: Charge indicator spawned"));
}
void UPlayerSkillComponent::SpawnChargeIndicator(FVector Start,FVector End)
{
	if(!ChargeDecalMaterial)
	{
		UE_LOG(LogTemp,Warning,TEXT("ChargeDecalMaterial is nullptr!"));
		return;
	} 
	if(!ChargeDecalComponent)
	{
		UE_LOG(LogTemp,Warning,TEXT("ChargeDecalComponent is nullptr!"));

		ChargeDecalComponent = NewObject<UDecalComponent>(this);
		ChargeDecalComponent->RegisterComponent();
		ChargeDecalComponent->AttachToComponent(Cast<AActor>(GetOwner())->GetRootComponent(),FAttachmentTransformRules::KeepWorldTransform);
		ChargeDecalComponent->SetDecalMaterial(ChargeDecalMaterial);
	}
	FVector MidPoint = (Start + End) * 0.5f;
	float Length = FVector::Distance(Start,End);
	ChargeDecalComponent->DecalSize = FVector(150.f,Length * 0.5f,100.f); //데칼의 두께, 길이, 넓이

	// 데칼 방향: 돌진 방향으로 정렬 (Pitch -90은 지면 투사)
		// Z축 투사, Y축 방향 보정 (Yaw + 90도)
	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(Start,End);
	FRotator DecalRot = FRotator(-90.f,LookAtRot.Yaw + 90.f,0.f);
	
	ChargeDecalComponent->SetWorldLocation(MidPoint);
	ChargeDecalComponent->SetWorldRotation(DecalRot);
}
void UPlayerSkillComponent::ExecuteChargeDash(FVector Chargedistance, FString SkillName)
{
	// 준비 시간 동안 저장된 dash 방향을 사용
	ACharacter* MyChar = Cast<ACharacter>(GetOwner());
	if (!MyChar) return;

	FVector DashDirection = StoredDashDirection;
	VisibleShapeBox(SkillName);
	// 현재 DamagedActors를 안전하게 복사해서 사용
// ExecuteChargeDash 내부에서

	SkillEffect(SkillName);
	float DashSpeed = 2000.0f;

	MyChar->LaunchCharacter(DashDirection * DashSpeed, true, true);
	UE_LOG(LogTemp, Warning, TEXT("ExecuteChargeDash: Dashing"));

	// 넉백 효과를 살짝 딜레이해서 실행 (0.2초 뒤)
	FTimerHandle KnockbackTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		KnockbackTimerHandle,
		FTimerDelegate::CreateUObject(this, &UPlayerSkillComponent::DelayedKnockbackEffect, SkillName), 0.2f, false);
}
void UPlayerSkillComponent::DelayedKnockbackEffect(FString SkillName)
{
	HideHitBox();
	// PlayerSkillComponent에서 관리하는 DamagedActors 목록을 사용
	for (AActor* DamagedActor : DamagedActors)
	{
		if (IsValid(DamagedActor))
		{
			// 넉백 처리
			UE_LOG(LogTemp, Warning, TEXT("DelayedKnockbackEffect 실행됨: %s"), *DamagedActor->GetName());
		}
	}
}
//<원숭이>
void UPlayerSkillComponent::SpawnProjectile_ThrowRock()
{
	// 스폰할 투사체 클래스 설정 확인
	if(!NomalProjectileClass)
	{
		UE_LOG(LogTemp,Error,TEXT("NomalProjectileClass not set!"));
		return;
	}

	// 소켓에서 스폰 위치 및 방향 획득
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if(!OwnerCharacter) return;

	USkeletalMeshComponent* MeshComp = OwnerCharacter->GetMesh();
	if(!MeshComp) return;

	FVector SpawnLocation = MeshComp->GetSocketLocation(TEXT("ThrowRockSocket"));
	FVector Direction = MeshComp->GetRightVector(); // 메시 기준 전방이 RightVector라고 하셨으므로

	// 스폰 파라미터
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = OwnerCharacter;
	SpawnParams.Instigator = OwnerCharacter;

	// 투사체 스폰
	APlayerProjectile* SpawnedProjectile = GetWorld()->SpawnActor<APlayerProjectile>(
		NomalProjectileClass,SpawnLocation,Direction.Rotation(),SpawnParams
	);

	if(SpawnedProjectile)
	{
		FSkillData SkillData;
		TArray<FSkillEffectData> EffectDataArray;

		if(UABGameSingleton::Get().GetSkillDataBySkillID("Skill_ThrowRock",SkillData) &&
			UABGameSingleton::Get().GetSkillEffectDataBySkillID("Skill_ThrowRock",EffectDataArray))
		{
			SpawnedProjectile->InitProjectileBySkillData(SkillData,EffectDataArray);
			SpawnedProjectile->FireInDirection(Direction);
			UE_LOG(LogTemp,Warning,TEXT("amam Spawned PlayerProjectile for Skill_ThrowRock"));
		} else
		{
			UE_LOG(LogTemp,Error,TEXT("amam Failed to get Skill_ThrowRock data!"));
		}
	} else
	{
		UE_LOG(LogTemp,Error,TEXT("amam Failed to spawn PlayerProjectile!"));
	}
}

void UPlayerSkillComponent::SkillAnimation(const FString& EffectID)
{
	//UE_LOG(LogTemp, Warning, TEXT("On SkillAnimation"));
	AActor* OwnerActor = GetOwner();
	if (OwnerActor && OwnerActor->IsA<ACharacter>())
	{
		ACharacter* CharacterOwner = Cast<ACharacter>(OwnerActor);
		UAnimInstance* AnimInstance = CharacterOwner->GetMesh()->GetAnimInstance();

		UActionAnimInstance* ActionAnimInstance = Cast<UActionAnimInstance>(AnimInstance);
		if (ActionAnimInstance)
		{
			ActionAnimInstance->PlayAnimation(EffectID);

			// 델리게이트 바인딩 추가
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &UPlayerSkillComponent::EndSkillAnimation);
			ActionAnimInstance->Montage_SetEndDelegate(EndDelegate);
			UCharacterStateComponent* StateComp = OwnerActor->FindComponentByClass<UCharacterStateComponent>();
			if (StateComp)
			{
				StateComp->ChangeState(ECharacterState::Action);
			}
		}
	}
}
void UPlayerSkillComponent::EndSkillAnimation(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Skill Animation Ended: %s"), *Montage->GetName());
		HideHitBox();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Skill Animation Ended: Montage is null."));
	}
	AN_Graduation_projectCharacter* MyChar = GetOwner<AN_Graduation_projectCharacter>();
	MyChar->EndAction();
}


void UPlayerSkillComponent::SkillEffect(const FString& SkillNameID)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("SkillEffect 실행됨"));

	TArray<FSkillEffectData> EffectData;
	if (!UABGameSingleton::Get().GetSkillEffectDataBySkillID(SkillNameID, EffectData))
	{
		UE_LOG(LogTemp, Warning, TEXT("no Effect: %s"), *SkillNameID);
		return;
	}
	AN_Graduation_projectCharacter* MyChar = GetOwner<AN_Graduation_projectCharacter>();
	if (!MyChar) return;
	//DamageAmount = EffectData[0].EffectValue01;
	SnapshotDamagedActors = DamagedActors.Array();
	UE_LOG(LogTemp, Warning, TEXT("SkillEffect_ 실행 직전 SnapshotDamagedActors 수: %d"), SnapshotDamagedActors.Num());
	for (AActor* TargetActor : SnapshotDamagedActors)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillEffect_ 실행 후 SnapshotDamagedActors 수: %d"), SnapshotDamagedActors.Num());

		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("AActor* TargetActor : DamagedActors이거 실행됨"));
		if (!IsValid(TargetActor)) continue;

		for (const FSkillEffectData& Effect : EffectData)
		{
			UEnum* EnumPtr = StaticEnum<EnumEffectType>();
			if(EnumPtr)
			{
				FString EnumName = EnumPtr->GetNameStringByIndex(static_cast<int32>(Effect.EffectType));
				UE_LOG(LogTemp,Warning,TEXT("EffectData : %s"),*EnumName);
			}



			if (Effect.EffectType == EnumEffectType::Damage)
			{
				if (MyChar->TestMode == false) {
					// 데미지
					DamageAmount = Effect.EffectValue01;
					UGameplayStatics::ApplyDamage(TargetActor, DamageAmount, MyChar->GetController(), MyChar, nullptr);
					UE_LOG(LogTemp, Warning, TEXT("ㅊㅊㅊ %s Damage: %f"), *TargetActor->GetName(), DamageAmount);
					GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Damage 실행됨"));
				}
				else {
					DamageAmount = Effect.EffectValue01;
					UGameplayStatics::ApplyDamage(TargetActor, 10000, MyChar->GetController(), MyChar, nullptr);
					UE_LOG(LogTemp, Warning, TEXT("ㅊㅊㅊ %s Damage: %f"), *TargetActor->GetName(), DamageAmount);
					GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Damage 실행됨"));

				}
			}
			if (Effect.EffectType == EnumEffectType::KnockBack)
			{
				// 넉백
				ApplyKnockback(TargetActor, Effect.EffectValue01);
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("넉백 실행됨"));

			}
		}
		// 그 후 데미지 적용
		UE_LOG(LogTemp, Warning, TEXT("SkillEffect 실행됨! DamageAmount: %f"), DamageAmount);

	}
}

void UPlayerSkillComponent::ApplyKnockback(AActor* TargetActor, float KnockbackPower)
{
	if (!IsValid(TargetActor)) return;

	FVector KnockbackDir = TargetActor->GetActorLocation() - GetOwner()->GetActorLocation();
	//KnockbackDir.Z = 0.f; // 위로 튀지 않게 평면 넉백
	KnockbackDir.Normalize();

	if (ACharacter* TargetChar = Cast<ACharacter>(TargetActor))
	{
		UCharacterMovementComponent* MoveComp = TargetChar->GetCharacterMovement();

		// AI 컨트롤러 얻기
		AAIController* AICon = Cast<AAIController>(TargetChar->GetController());
		if (AICon)
		{
			// 이동 중지
			AICon->StopMovement();
			MoveComp->StopMovementImmediately();

			// 넉백 실행
			TargetChar->LaunchCharacter(KnockbackDir * KnockbackPower, true, true);

		}
		else
		{
			// AI 없는 경우에도 넉백 적용
			TargetChar->LaunchCharacter(KnockbackDir * KnockbackPower, true, true);
		}
		if (AController* Ctrl = TargetChar->GetController())
		{
			UE_LOG(LogTemp, Warning, TEXT("넉백 Controller 클래스: %s"), *Ctrl->GetClass()->GetName());
		}
		UE_LOG(LogTemp, Warning, TEXT("넉백 성공: %s 방향 %s 파워 %f"), *TargetChar->GetName(), *KnockbackDir.ToString(), KnockbackPower);
		HideHitBox();
	}
}
