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
#include "Particles/ParticleSystemComponent.h" 
#include "MyKnockbackBall.h"
#include "Barrel.h"
#include "StunBarrel.h"
#include "FireBarrel.h"
#include "EntityPreset.h"
#include "FireFloor.h"
#include "FrozeFloor.h"
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
	static ConstructorHelpers::FClassFinder<APlayerProjectile> SProjectileBP(TEXT("/Game/Entity/BP/BP_Player_Inpermon_Projectile_FireBall"));
	if(SProjectileBP.Succeeded())
	{
		SpecialProjectileClass = SProjectileBP.Class;
	}

	//실드 이펙트
	ShieldParticle = TSoftObjectPtr<UParticleSystem>(FSoftObjectPath(TEXT("/Game/Asset/FXAsset/LoPoPack/Particles/Par_LoPo_Shield_01.Par_LoPo_Shield_01")));
	// TSoftObjectPtr에서 실제로 로드할 수 있도록 LoadSynchronous 사용
	ShieldParticle.LoadSynchronous();
	if(ShieldParticle.IsValid())
	{
		// 로드된 후 유효한 경우
		UE_LOG(LogTemp,Warning,TEXT("ShieldParticle 있음"));
	} else
	{
		// 로드되지 않은 경우
		UE_LOG(LogTemp,Warning,TEXT("ShieldParticle 없음"));
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
	ChargeDecalComponent->SetVisibility(false);
}
void UPlayerSkillComponent::SetHitBox(UBoxComponent* InHitBox)
{
	PlayerHitBox = InHitBox;
	UE_LOG(LogTemp,Warning,TEXT("papago PlayerHitBox Address: %p"),PlayerHitBox);

}
void UPlayerSkillComponent::SetHitBox2(UBoxComponent* InHitBox)
{
	PlayerHitBox2 = InHitBox;
	UE_LOG(LogTemp,Warning,TEXT("papago PlayerHitBox2 Address: %p"),PlayerHitBox2);

}
/* 스킬 관련 */
void UPlayerSkillComponent::OnDefenseSkill()
{
	IsDefending = true;
	auto StatComponent = GetOwner()->FindComponentByClass<UWidgetActor>();

	StatComponent->HUDWidget->SkeletonGuard=true;
	if(StatComponent && StatComponent->HUDWidget)
	{
		StatComponent->HUDWidget->bHolding=true;
	}
}

void UPlayerSkillComponent::OffDefenseSkill()
{
	IsDefending = false;
	UE_LOG(LogTemp,Warning,TEXT("OffDefenseSkill실행됨"));

	//스킬 쿭타임
	auto StatComponent = GetOwner()->FindComponentByClass<UWidgetActor>();
	if(StatComponent && StatComponent->HUDWidget)
	{
		StatComponent->HUDWidget->UpdateSpecialSkillCooldown(0.5,CanUseNomalSkill,CanUseSpecialSkill);
		StatComponent->HUDWidget->CanSpecial = false;
		StatComponent->HUDWidget->bHolding=false;
	}
	//몽타주 재개
	AActor* OwnerActor = GetOwner();
	ACharacter* CharacterOwner = Cast<ACharacter>(OwnerActor);

	if(ShieldParticle.IsValid()){
		if(UAnimInstance* AnimInstance = CharacterOwner->GetMesh()->GetAnimInstance())
		{
			if(UAnimMontage* PausedMontage = AnimInstance->GetCurrentActiveMontage())
			{
				AnimInstance->Montage_Resume(PausedMontage);
				UE_LOG(LogTemp,Warning,TEXT("방어 해제 → 몽타주 다시 재생됨: %s"),*PausedMontage->GetName());
			} else
			{
				UE_LOG(LogTemp,Warning,TEXT("재생 중인 몽타주가 없습니다."));
			}
		}
	}
	//실드 이펙트
	if(ShieldParticleComp) {
		ShieldParticleComp->DestroyComponent();  // 이펙트 삭제
		ShieldParticleComp = nullptr;  // 변수 초기화
	}
}
void UPlayerSkillComponent::SetSkillTimer(float Count,FTimerDelegate End)
{
	if(GetWorld() && Count > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(NomalSkillTimerHandle,End,Count,false);
		UE_LOG(LogTemp,Log,TEXT("hum SetSkillTimer사용됨"));
	}
}

void UPlayerSkillComponent::SpecialSetSkillTimer(float Count,FTimerDelegate End)
{
	if(GetWorld() && Count > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(SpecialSkillTimerHandle,End,Count,false);
		UE_LOG(LogTemp,Log,TEXT("OnMontag SpecialSetSkillTimer사용됨"));
	}
}
void UPlayerSkillComponent::ChargeSkillTimer(float Delay,FTimerDelegate End)
{
	if(GetWorld() && Delay > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(ChargeSkillTimerHandle,End,Delay,false);
		UE_LOG(LogTemp,Log,TEXT("ChargeSkillTimer 사용됨"));
	}
}

//쿨타임
void UPlayerSkillComponent::NomalCooldown()
{
	CanUseNomalSkill = true;

	UE_LOG(LogTemp,Log,TEXT("Kakao NomalCooldown"));
}
void UPlayerSkillComponent::SpecialCooldown()
{
	CanUseSpecialSkill = true;
}

//히트박스
void UPlayerSkillComponent::SettingHitBox(const FSkillData& SkillData)
{
	if(!OnceHitBox && PlayerHitBox)
	{
		// 히트박스 크기 및 위치 설정
		FVector NewBoxExtent = FVector(SkillData.SkillTypeSizeX,SkillData.SkillTypeSizeY,100);
		PlayerHitBox->SetBoxExtent(NewBoxExtent);

		FVector NewLocation = PlayerHitBox->GetRelativeLocation();
		NewLocation.X += SkillData.SkillTypeSizeX;
		PlayerHitBox->SetRelativeLocation(NewLocation);

		OnceHitBox = true;
	}
}
void UPlayerSkillComponent::SettingHitBox2(const FSkillData& SkillData)
{
	if(PlayerHitBox2)
	{
		UE_LOG(LogTemp,Log,TEXT("SettingHitBox2"));

		FVector NewBoxExtent2 = FVector(SkillData.SkillTypeSizeX,SkillData.SkillTypeSizeY,100);
		PlayerHitBox2->SetBoxExtent(NewBoxExtent2);

		// 항상 일정한 기준 위치에서 상대 이동
		FVector BaseOffset = FVector(SkillData.SkillTypeSizeX,0.f,0.f);
		PlayerHitBox2->SetRelativeLocation(BaseOffset);

		OnceHitBox = true;
	}
}
void UPlayerSkillComponent::OnHitBox(const FSkillData& SkillData)
{
	if(PlayerHitBox)
	{
		PlayerHitBox->SetVisibility(true);
		PlayerHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);  // 충돌 키기

		UE_LOG(LogTemp,Warning,TEXT("[OnHitBox] On OnHitBox"));
		UE_LOG(LogTemp,Warning,TEXT("[OnHitBox] On PlayerHitBox  Address: %p"),PlayerHitBox);

	} else
	{
		UE_LOG(LogTemp,Error,TEXT("[OnHitBox] Failed to find HitBox "));
		UE_LOG(LogTemp,Warning,TEXT("[OnHitBox] On Failed PlayerHitBox Address: %p"),PlayerHitBox);

	}
}
void UPlayerSkillComponent::OnHitBox2(const FSkillData& SkillData)
{
	if(PlayerHitBox2)
	{
		PlayerHitBox2->SetVisibility(true);
		PlayerHitBox2->SetCollisionEnabled(ECollisionEnabled::QueryOnly);  // 충돌 키기

		UE_LOG(LogTemp,Warning,TEXT("[OnHitBox2] On OnHitBox"));
		UE_LOG(LogTemp,Warning,TEXT("[OnHitBox2] On PlayerHitBox  Address: %p"),PlayerHitBox2);

	} else
	{
		UE_LOG(LogTemp,Error,TEXT("[OnHitBox] Failed to find HitBox "));
		UE_LOG(LogTemp,Warning,TEXT("[OnHitBox] On Failed PlayerHitBox Address: %p"),PlayerHitBox2);

	}
}
void UPlayerSkillComponent::HideHitBox()
{
	PlayerHitBox->SetVisibility(false);  // 자식까지 숨기기
	PlayerHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);  // 충돌 꺼버리기
	// 스킬 컴포넌트 내부에서 소유 액터를 통해 접근
	UE_LOG(LogTemp,Warning,TEXT("papago hidden PlayerHitBox  Address: %p"),PlayerHitBox);

	// 데미지 체크 초기화
	DamagedActors.Empty();
}
void UPlayerSkillComponent::HideHitBox2()
{
	PlayerHitBox2->SetVisibility(false);  // 자식까지 숨기기
	PlayerHitBox2->SetCollisionEnabled(ECollisionEnabled::NoCollision);  // 충돌 꺼버리기
	// 스킬 컴포넌트 내부에서 소유 액터를 통해 접근

	UE_LOG(LogTemp,Warning,TEXT("papago hidden PlayerHitBox2  Address: %p"),PlayerHitBox2);

	// 데미지 체크 초기화
	DamagedActors.Empty();
}

AActor* UPlayerSkillComponent::FindFrontMonsterTarget() const
{
	if(!GetWorld()) return nullptr;

	AActor* ClosestMonster = nullptr;
	float MinDistance = FLT_MAX;

	AActor* Owner = GetOwner();
	if(!Owner) return nullptr;

	FVector PlayerLocation = Owner->GetActorLocation();

	for(TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if(Actor && Actor->ActorHasTag(FName("Monster"))|| Actor->ActorHasTag(FName("Objcet")))
		{
			float CurrentDistance = FVector::Dist(PlayerLocation,Actor->GetActorLocation());

			if(CurrentDistance < MinDistance)
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
	if(MonsterTarget)
	{
		UE_LOG(LogTemp,Warning,TEXT("GetDistanceTo(MonsterTarget): %f"),GetDistanceTo(MonsterTarget));
		return GetDistanceTo(MonsterTarget);
	}
	return 0.f;
}

/* 스킬 실행 */
void UPlayerSkillComponent::VisibleShapeBox(const FString& SkillID)
{
	FSkillData SkillData;
	if(!UABGameSingleton::Get().GetSkillDataBySkillID(SkillID,SkillData)) return;
	if(SkillData.SkillType == EnumSkillType::HitBox)
	{
		UE_LOG(LogTemp,Warning,TEXT("HitBox 실행됨"));

		if(SkillData.SkillTypeShape == EnumSkillTypeShape::Box)
		{
			if(PlayerHitBox){
				SettingHitBox(SkillData);  // 히트박스 초기화
				OnHitBox(SkillData);    // 히트박스 활성화
			}

		} else if(SkillData.SkillTypeShape == EnumSkillTypeShape::Sphere)
		{
			//	 Sphere 관련 처리 추가
		}
	} else if(SkillData.SkillType == EnumSkillType::Projectile)
	{
		UE_LOG(LogTemp,Warning,TEXT("Projectile 실행됨"));
	} else
	{
		//OnDefenseSkill();
	}
}
void UPlayerSkillComponent::NomalSkillPlay(const FString& SkillID)
{
	distance = MeasureDistanceToMonster();

	if(CanUseNomalSkill)
	{
		bIsSpecialAttack = false;

		FSkillData SkillData;
		if(!UABGameSingleton::Get().GetSkillDataBySkillID(SkillID,SkillData)) return;

		AN_Graduation_projectCharacter* MyChar = GetOwner<AN_Graduation_projectCharacter>();
		MyChar->StartAction();

		CurrentSkillID = SkillID;

		UE_LOG(LogTemp,Warning,TEXT("KKakao Distance to Monster: %f"),distance);

		UE_LOG(LogTemp,Warning,TEXT("%f Kkakao Yes distance"),SkillData.SkillRange);
		VisibleShapeBox(SkillID); // 적중 범위 표시
		SkillEffect(SkillID);

		// 스킬 쿨타임 시작
		CanUseNomalSkill = false;

		auto StatComponent = GetOwner()->FindComponentByClass<UWidgetActor>();
		if(StatComponent && StatComponent->HUDWidget)
		{
			StatComponent->HUDWidget->UpdateNomalSkillCooldown(SkillData.SkillCoolTime,CanUseNomalSkill,CanUseSpecialSkill);
			StatComponent->HUDWidget->CanNomal = false;
		}

		// 스킬 애니메이션
		SkillAnimation(SkillID);
		UE_LOG(LogTemp,Warning,TEXT("amam OnMontag Playing %s"),*SkillID);

		HideHitBox();
		// 쿨타임 타이머 설정
		FTimerDelegate NomalCooldownEnd;
		NomalCooldownEnd.BindUObject(this,&UPlayerSkillComponent::NomalCooldown);
		SetSkillTimer(SkillData.SkillCoolTime,NomalCooldownEnd);
	} else
	{
		UE_LOG(LogTemp,Warning,TEXT("kakao No CanUseNomalSkill"));
	}
}

void UPlayerSkillComponent::SpecialSkillPlay(const FString& SkillID)
{
	//UE_LOG(LogTemp, Warning, TEXT("kakao On SpecialSkillPlay"));
	distance = MeasureDistanceToMonster();

	if(CanUseSpecialSkill)
	{
		bIsSpecialAttack = true;
		FSkillData SkillData;
		if(!UABGameSingleton::Get().GetSkillDataBySkillID(SkillID,SkillData)) return;

		AN_Graduation_projectCharacter* MyChar = GetOwner<AN_Graduation_projectCharacter>();
		MyChar->StartAction();

		CurrentSkillID = SkillID;
		if(SkillID == "Skill_Charge")
		{
			auto StatComponent = GetOwner()->FindComponentByClass<UWidgetActor>();

			StatComponent->HUDWidget->SkeletonGuard=true;
			if(StatComponent && StatComponent->HUDWidget)
			{
				StatComponent->HUDWidget->bHolding=true;
			}

			StoredDashDirection = MyChar->GetActorForwardVector().GetSafeNormal();

			DrawChargePath(); // 돌진 선

			float PrepTime = 1.0f;

			FTimerDelegate ChargeEnd;
			ChargeEnd.BindUFunction(this,FName("ExecuteChargeDash"),StoredDashDirection,SkillID);
			ChargeSkillTimer(PrepTime,ChargeEnd);
			// 범위 내일 때만 히트 판정 박스 표시
		}
		if(SkillID=="Skill_FreezeBreath"){
			SettingHitBox2(SkillData);  // 히트박스 초기화
			OnHitBox2(SkillData);    // 히트박스 활성화	
			SkillEffect(SkillID);
		}

		auto StatComponent = GetOwner()->FindComponentByClass<UWidgetActor>();
		if(StatComponent && StatComponent->HUDWidget)
		{
			StatComponent->HUDWidget->UpdateSpecialSkillCooldown(SkillData.SkillCoolTime,CanUseNomalSkill,CanUseSpecialSkill);
			StatComponent->HUDWidget->CanSpecial = false;
		}

		if(SkillID == "Skill_ShieldGuard") {
			OnDefenseSkill();
			//실드 이펙트
			if(ShieldParticle.IsValid())
			{
				ShieldParticleComp = UGameplayStatics::SpawnEmitterAttached(ShieldParticle.Get(),
					MyChar->GetMesh(),                // 붙일 대상: 캐릭터 메시
					FName(NAME_None),        // 본 이름 (NAME_None= 전체 바디)
					FVector::ZeroVector,     // 오프셋
					FRotator::ZeroRotator,
					EAttachLocation::KeepRelativeOffset,
					false                   // 소멸 시 자동 제거 여부
				);
				if(ShieldParticleComp == nullptr)
				{
					UE_LOG(LogTemp,Warning,TEXT("Failed to spawn shield particle."));
				}

			}
		}
		CanUseSpecialSkill = false;

		// 스킬 애니메이션
		SkillAnimation(SkillID);
		UE_LOG(LogTemp,Warning,TEXT("OnMontag Playing %s"),*SkillID);

		FTimerDelegate SpecialCooldownEnd;
		SpecialCooldownEnd.BindUObject(this,&UPlayerSkillComponent::SpecialCooldown);
		SpecialSetSkillTimer(SkillData.SkillCoolTime,SpecialCooldownEnd);
	} else
	{
		UE_LOG(LogTemp,Warning,TEXT("kakao No CanUseSpecialSkill"));
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
		ChargeDecalComponent->SetVisibility(true);

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
void UPlayerSkillComponent::ExecuteChargeDash(FVector Chargedistance,FString SkillName)
{
	// 준비 시간 동안 저장된 dash 방향을 사용
	ACharacter* MyChar = Cast<ACharacter>(GetOwner());
	if(!MyChar) return;

	FVector DashDirection = StoredDashDirection;
	VisibleShapeBox(SkillName);
	auto StatComponent = GetOwner()->FindComponentByClass<UWidgetActor>();

	StatComponent->HUDWidget->SkeletonGuard=true;
	if(StatComponent && StatComponent->HUDWidget)
	{
		StatComponent->HUDWidget->bHolding=false;
	}
	// 현재 DamagedActors를 안전하게 복사해서 사용
// ExecuteChargeDash 내부에서

	SkillEffect(SkillName);
	float DashSpeed = 2000.0f;

	MyChar->LaunchCharacter(DashDirection * DashSpeed,true,true);
	UE_LOG(LogTemp,Warning,TEXT("ExecuteChargeDash: Dashing"));

	// 넉백 효과를 살짝 딜레이해서 실행 (0.2초 뒤)
	FTimerHandle KnockbackTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		KnockbackTimerHandle,
		FTimerDelegate::CreateUObject(this,&UPlayerSkillComponent::DelayedKnockbackEffect,SkillName),0.2f,false);
}
void UPlayerSkillComponent::DelayedKnockbackEffect(FString SkillName)
{
	HideHitBox();
	// PlayerSkillComponent에서 관리하는 DamagedActors 목록을 사용
	for(AActor* DamagedActor : DamagedActors)
	{
		if(IsValid(DamagedActor))
		{
			// 넉백 처리
			UE_LOG(LogTemp,Warning,TEXT("DelayedKnockbackEffect 실행됨: %s"),*DamagedActor->GetName());
		}
	}
}
void UPlayerSkillComponent::ApplyKnockback(AActor* TargetActor,float Distance,float Duration)
{
	if(!IsValid(TargetActor)) return;

	FVector KnockbackDir = TargetActor->GetActorLocation() - GetOwner()->GetActorLocation();
	//KnockbackDir.Z = 0.f; // 위로 튀지 않게 평면 넉백
	KnockbackDir.Normalize();
	float KnockbackSpeed = Distance / Duration;
	FVector KnockbackVelocity = KnockbackDir * KnockbackSpeed;

	if(ACharacter* TargetChar = Cast<ACharacter>(TargetActor))
	{
		UCharacterMovementComponent* MoveComp = TargetChar->GetCharacterMovement();

		// AI 컨트롤러 얻기
		AAIController* AICon = Cast<AAIController>(TargetChar->GetController());
		if(AICon)
		{
			// 이동 중지
			AICon->StopMovement();
			MoveComp->StopMovementImmediately();

			// 넉백 실행
			TargetChar->LaunchCharacter(KnockbackVelocity,true,true);
			UE_LOG(LogTemp,Warning,TEXT("넉백 KnockbackVelocity: %s"),*KnockbackVelocity.ToString());

		} else
		{
			// AI 없는 경우에도 넉백 적용
			TargetChar->LaunchCharacter(KnockbackVelocity,true,true);
		}
		if(AController* Ctrl = TargetChar->GetController())
		{
			//UE_LOG(LogTemp,Warning,TEXT("넉백 Controller 클래스: %s"),*Ctrl->GetClass()->GetName());
		}
		//	UE_LOG(LogTemp,Warning,TEXT("넉백 성공: %s 방향 %s 파워 %f"),*TargetChar->GetName(),*KnockbackDir.ToString(),KnockbackPower);

		HideHitBox();
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
	FVector Direction = MeshComp->GetRightVector(); // 메시가 270도 회전된 상태가 X축 전방이기 때문에 Right Vector를 가져옴 

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

void UPlayerSkillComponent::SpawnProjectile_FireBall()
{
	// 스폰할 투사체 클래스 설정 확인
	if(!SpecialProjectileClass)
	{
		UE_LOG(LogTemp,Error,TEXT("SpecialProjectileClass not set!"));
		return;
	}

	// 소켓에서 스폰 위치 및 방향 획득
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if(!OwnerCharacter) return;

	USkeletalMeshComponent* MeshComp = OwnerCharacter->GetMesh();
	if(!MeshComp) return;

	FVector SpawnLocation = MeshComp->GetSocketLocation(TEXT("ThrowRockSocket"));
	FVector Direction = MeshComp->GetRightVector(); // 메시가 270도 회전된 상태가 X축 전방이기 때문에 Right Vector를 가져옴 

	// 스폰 파라미터
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = OwnerCharacter;
	SpawnParams.Instigator = OwnerCharacter;

	// 투사체 스폰
	APlayerProjectile* SpawnedProjectile = GetWorld()->SpawnActor<APlayerProjectile>(
		SpecialProjectileClass,SpawnLocation,Direction.Rotation(),SpawnParams
	);

	if(SpawnedProjectile)
	{
		FSkillData SkillData;
		TArray<FSkillEffectData> EffectDataArray;

		if(UABGameSingleton::Get().GetSkillDataBySkillID("Skill_FireBall",SkillData) &&
			UABGameSingleton::Get().GetSkillEffectDataBySkillID("Skill_FireBall",EffectDataArray))
		{
			SpawnedProjectile->InitProjectileBySkillData(SkillData,EffectDataArray);
			SpawnedProjectile->FireInDirection(Direction);
			UE_LOG(LogTemp,Warning,TEXT("amam Spawned PlayerProjectile for Skill_FireBall"));
		} else
		{
			UE_LOG(LogTemp,Error,TEXT("amam Failed to get Skill_FireBall data!"));
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
	if(OwnerActor && OwnerActor->IsA<ACharacter>())
	{
		ACharacter* CharacterOwner = Cast<ACharacter>(OwnerActor);
		UAnimInstance* AnimInstance = CharacterOwner->GetMesh()->GetAnimInstance();

		UActionAnimInstance* ActionAnimInstance = Cast<UActionAnimInstance>(AnimInstance);
		if(ActionAnimInstance)
		{
			ActionAnimInstance->PlayAnimation(EffectID);

			UCharacterStateComponent* StateComp = OwnerActor->FindComponentByClass<UCharacterStateComponent>();
			if(StateComp)
			{
				StateComp->ChangeState(ECharacterState::Action);
			}
		}
	}
}

void UPlayerSkillComponent::EndSkillAnimation(UAnimMontage* Montage,bool bInterrupted)
{
	if(Montage)
	{
		UE_LOG(LogTemp,Warning,TEXT("Skill Animation Ended: %s"),*Montage->GetName());
		HideHitBox();
		HideHitBox2();

	} else
	{
		UE_LOG(LogTemp,Warning,TEXT("Skill Animation Ended: Montage is null."));
	}

	if(bIsSpecialAttack)
	{
		bIsSpecialAttack = false;
	}

	AN_Graduation_projectCharacter* MyChar = GetOwner<AN_Graduation_projectCharacter>();
	MyChar->EndAction();
}

void UPlayerSkillComponent::SkillEffect(const FString& SkillNameID)
{
	GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Red,TEXT("SkillEffect 실행됨"));

	TArray<FSkillEffectData> EffectData;
	if(!UABGameSingleton::Get().GetSkillEffectDataBySkillID(SkillNameID,EffectData))
	{
		UE_LOG(LogTemp,Warning,TEXT("no Effect: %s"),*SkillNameID);
		return;
	}
	AN_Graduation_projectCharacter* MyChar = GetOwner<AN_Graduation_projectCharacter>();
	if(!MyChar) return;
	//DamageAmount = EffectData[0].EffectValue01;
	TArray<AActor*> SnapshotDamagedActors = DamagedActors.Array(); // 지역 변수로!
	UE_LOG(LogTemp,Warning,TEXT("SkillEffect_ 실행 직전 SnapshotDamagedActors 수: %d"),SnapshotDamagedActors.Num());
	for(AActor* TargetActor : SnapshotDamagedActors)
	{
		UE_LOG(LogTemp,Warning,TEXT("SkillEffect_ 실행 후 SnapshotDamagedActors 수: %d"),SnapshotDamagedActors.Num());

		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("AActor* TargetActor : DamagedActors이거 실행됨"));
		if(!IsValid(TargetActor)) continue;

		for(const FSkillEffectData& Effect : EffectData)
		{
			UEnum* EnumPtr = StaticEnum<EnumEffectType>();
			if(EnumPtr)
			{
				FString EnumName = EnumPtr->GetNameStringByIndex(static_cast<int32>(Effect.EffectType));
				UE_LOG(LogTemp,Warning,TEXT("EffectData : %s"),*EnumName);
			}

			if(Effect.EffectType == EnumEffectType::Damage)
			{
				DamageAmount = Effect.EffectValue01;
				if(MyChar->TestMode == false) {
					// 데미지
					UGameplayStatics::ApplyDamage(TargetActor,DamageAmount,MyChar->GetController(),MyChar,nullptr);
					UE_LOG(LogTemp,Warning,TEXT("ㅊㅊㅊ %s Damage: %f"),*TargetActor->GetName(),DamageAmount);
					if(TargetActor->ActorHasTag(FName("Barrel")))
					{
						UE_LOG(LogTemp,Warning,TEXT("Barrel이여 작동하거라"));
						if(TargetActor->ActorHasTag(FName("Barrel")))
						{
							if(AStunBarrel* StunBarrel = Cast<AStunBarrel>(TargetActor))
							{
								StunBarrel->WorkBarrel(DamageAmount); // AStunBarrel 고유 로직
							} else if(ABarrel* NormalBarrel = Cast<ABarrel>(TargetActor))
							{
								NormalBarrel->WorkBarrel(DamageAmount); // ABarrel 로직
							} else if(AFireBarrel* FireBarrel = Cast<AFireBarrel>(TargetActor)){
								FireBarrel->WorkBarrel(DamageAmount); // FireBarrel 로직

							}
						}
					}
					//GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Red,TEXT("Damage 실행됨"));
				} else {
					UGameplayStatics::ApplyDamage(TargetActor,10000,MyChar->GetController(),MyChar,nullptr);
					UE_LOG(LogTemp,Warning,TEXT("ㅊㅊㅊ %s Damage: %f"),*TargetActor->GetName(),DamageAmount);
					if(TargetActor->ActorHasTag(FName("Barrel")))
					{
						if(AStunBarrel* StunBarrel = Cast<AStunBarrel>(TargetActor))
						{
							StunBarrel->WorkBarrel(DamageAmount); // AStunBarrel 고유 로직
						} else if(ABarrel* NormalBarrel = Cast<ABarrel>(TargetActor))
						{
							NormalBarrel->WorkBarrel(DamageAmount); // ABarrel 로직
						} else if(AFireBarrel* FireBarrel = Cast<AFireBarrel>(TargetActor)){
							FireBarrel->WorkBarrel(DamageAmount); // FireBarrel 로직

						}
					}
				}

			}
			if(Effect.EffectType == EnumEffectType::KnockBack)
			{
				// 넉백
				float KnockbackDistance = Effect.EffectValue01;
				float KnockbackDuration = Effect.EffectValue02;

				ApplyKnockback(TargetActor,KnockbackDistance,KnockbackDuration);
				UE_LOG(LogTemp,Warning,TEXT("넉백 TargetActor: %s KnockbackDistance: %f KnockbackDuration: %f"),
					   *TargetActor->GetName(),KnockbackDistance,KnockbackDuration);

			}
			if(Effect.EffectType == EnumEffectType::Freezing)
			{
				float Duration = Effect.EffectValue01;
				float SlowFactor = Effect.EffectValue02;
				if(Duration > 0.f && SlowFactor > 0.f)
				{
					AEntityPreset* Entity = Cast<AEntityPreset>(TargetActor);
					if(Entity)
					{
						APawn* Pawn = Cast<APawn>(Entity);
						if(Pawn)
						{
							UCharacterMovementComponent* MoveComp = Pawn->FindComponentByClass<UCharacterMovementComponent>();
							if(MoveComp)
							{
								const float OriginalSpeed = MoveComp->MaxWalkSpeed;
								const float NewSpeed = OriginalSpeed / SlowFactor;

								MoveComp->MaxWalkSpeed = NewSpeed;

								// 복구 타이머 설정
								FTimerHandle RestoreHandle;
								FTimerDelegate RestoreDelegate;

								// Pawn과 OriginalSpeed 캡처
								RestoreDelegate.BindLambda([=]() {
									if(Pawn && Pawn->FindComponentByClass<UCharacterMovementComponent>())
									{
										Pawn->FindComponentByClass<UCharacterMovementComponent>()->MaxWalkSpeed = OriginalSpeed;
										UE_LOG(LogTemp,Warning,TEXT("슬로우 해제: %s → 원래 속도 %.1f 복원됨"),*Pawn->GetName(),OriginalSpeed);
									}
								});

								// Duration 후 타이머 호출
								GetWorld()->GetTimerManager().SetTimer(RestoreHandle,RestoreDelegate,Duration,false);

								UE_LOG(LogTemp,Warning,TEXT("슬로우 적용: %s → 속도 %.1f → %.1f (%.1f초간)"),*Pawn->GetName(),OriginalSpeed,NewSpeed,Duration);
							}
						}
					}
				}
				if(TargetActor->ActorHasTag(FName("FireFloor")))
				{
					if(AFireFloor* FireFloor = Cast<AFireFloor>(TargetActor))
					{
						FireFloor->Off_Fire(); //화염 끔 코드
					} 
				}
				if(TargetActor->ActorHasTag(FName("FrozeFloor")))
				{
					if(AFrozeFloor* FrozeFloor = Cast<AFrozeFloor>(TargetActor))
					{
						FrozeFloor->On_Froze(); //빙결 킴 코드
					} 
				}


			}
		}
		// 그 후 데미지 적용
		UE_LOG(LogTemp,Warning,TEXT("SkillEffect 실행됨! DamageAmount: %f"),DamageAmount);

	}
}

