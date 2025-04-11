#include "PlayerSkillComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "MyPlayerStatComponent.h"
#include "WidgetActor.h"
#include "TimerManager.h"
#include "CharacterStateComponent.h" //state
#include "EngineUtils.h"
#include "N_Graduation_project/N_Graduation_projectCharacter.h"

UPlayerSkillComponent::UPlayerSkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	IsDefending = false;
	OnceHitBox = false;
	CanUseNomalSkill = true;
	CanUseSpecialSkill = true;
	DamageAmount = 50;

}

void UPlayerSkillComponent::BeginPlay()
{
	Super::BeginPlay();

}
void UPlayerSkillComponent::SetHitBox(UBoxComponent* InHitBox)
{
	PlayerHitBox = InHitBox;
	UE_LOG(LogTemp, Warning, TEXT("papago PlayerHitBox Address: %p"), PlayerHitBox);

}
/* 스킬 관련 */
void UPlayerSkillComponent::SetSkillTimer(float Count, FTimerDelegate End)
{
	if (Count > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, End, Count, false);
	}
}

void UPlayerSkillComponent::OnDefenseSkill(float Count)
{
	IsDefending = true;
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("DefenseSkill on"));
	}

	// 방어 해제 타이머 설정
	FTimerDelegate DefenseEnd;
	DefenseEnd.BindUObject(this, &UPlayerSkillComponent::OffDefenseSkill);
	SetSkillTimer(Count, DefenseEnd);
}

void UPlayerSkillComponent::OffDefenseSkill()
{
	IsDefending = false;
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("DefenseSkill off"));
}

void UPlayerSkillComponent::NomalCooldown()
{
	CanUseNomalSkill = true;

	auto StatComponent = GetOwner()->FindComponentByClass<UWidgetActor>();
	if (StatComponent && StatComponent->HUDWidget)
	{
		StatComponent->HUDWidget->CanNomal = CanUseNomalSkill;
	}

	UE_LOG(LogTemp, Log, TEXT("Kakao NomalCooldown"));
}
void UPlayerSkillComponent::SpecialCooldown()
{
	CanUseSpecialSkill = true;
	auto StatComponent = GetOwner()->FindComponentByClass<UWidgetActor>();
	if (StatComponent && StatComponent->HUDWidget)
	{
		StatComponent->HUDWidget->CanSpecial = CanUseNomalSkill;
	}
}

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
}

AActor* UPlayerSkillComponent::FindFrontMonsterTarget() const
{
	//지삐티니
	if (!GetWorld()) return nullptr;

	AActor* ClosestMonster = nullptr;//몬스터를 담을 변수
	float MinDistance = FLT_MAX;//현재까지 찾은 몬스터 중 가장 가까운 거리
	float MaxDotProduct = -1.0f; // 정면(1에 가까울수록 정면 방향)

	AActor* Owner = GetOwner();
	if (!Owner) return nullptr;

	FVector PlayerLocation = Owner->GetActorLocation();
	FVector PlayerForward = Owner->GetActorForwardVector(); // 플레이어 정면 방향

	for (TActorIterator<AActor> It(GetWorld()); It; ++It) //월드에 존재하는 모든 AActor를 순회하며 Monster 태그 확인
	{
		AActor* Actor = *It;
		if (Actor && Actor->ActorHasTag(FName("Monster")))
		{
			FVector ToMonster = (Actor->GetActorLocation() - PlayerLocation).GetSafeNormal();
			float DotProduct = FVector::DotProduct(PlayerForward, ToMonster); // 정면과의 유사도 계산

			float CurrentDistance = FVector::Dist(PlayerLocation, Actor->GetActorLocation());

			// 정면 방향이고, 기존보다 가까운 몬스터를 선택
			if (DotProduct > MaxDotProduct || (DotProduct == MaxDotProduct && CurrentDistance < MinDistance))
			{
				MaxDotProduct = DotProduct;
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
		//UE_LOG(LogTemp, Warning, TEXT("GetDistanceTo(MonsterTarget): %f"), GetDistanceTo(MonsterTarget));
		return GetDistanceTo(MonsterTarget);
	}
	return 0.f;
}

/* 스킬 실행 */
void UPlayerSkillComponent::VisibleShapeBox(const FString& SkillID)
{
	FSkillData SkillData;
	if (!UABGameSingleton::Get().GetSkillDataBySkillID(SkillID, SkillData)) return;

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
void UPlayerSkillComponent::NomalSkillPlay(const FString& SkillID)
{
	UE_LOG(LogTemp, Warning, TEXT("kakao On NomalSkillPlay"));
	distance = MeasureDistanceToMonster();
	UE_LOG(LogTemp, Warning, TEXT("Distance to Monster: %f"), distance);
	if (CanUseNomalSkill == true) {

		UE_LOG(LogTemp, Warning, TEXT("kakao On CanUseNomalSkill"));

		FSkillData SkillData;
		if (!UABGameSingleton::Get().GetSkillDataBySkillID(SkillID, SkillData)) return;

		if (distance <= SkillData.SkillRange)
		{
			AN_Graduation_projectCharacter* MyChar = GetOwner<AN_Graduation_projectCharacter>();
			MyChar->StartAction();

			UE_LOG(LogTemp, Warning, TEXT("kakao Yes distance"));

			VisibleShapeBox(SkillID);
			//스킬 쿨타임
			CanUseNomalSkill = false;
			auto StatComponent = GetOwner()->FindComponentByClass<UWidgetActor>();
			if (StatComponent && StatComponent->HUDWidget)
			{
				StatComponent->HUDWidget->UpdateSkillCooldown(SkillData.SkillCoolTime, CanUseNomalSkill, CanUseSpecialSkill);
				StatComponent->HUDWidget->CanNomal = false;
				UE_LOG(LogTemp, Log, TEXT("StatComponent: %p, HUDWidget: %p"), StatComponent, StatComponent ? StatComponent->HUDWidget : nullptr);
			}
			SkillAnimation(SkillID);

			//노말 스킬
		/*	if (SkillID == "Skill_Slash") {
				SkillAnimation(SkillID);
				UE_LOG(LogTemp, Warning, TEXT("kakao On SkillSlash"));

			}*/

			//쿨타임
			FTimerDelegate NomalCooldownEnd;
			NomalCooldownEnd.BindUObject(this, &UPlayerSkillComponent::NomalCooldown);//바인딩
			SetSkillTimer(SkillData.SkillCoolTime, NomalCooldownEnd);  // 쿨타임 설정
			//SetSkillTimer(3.0f, NomalCooldownEnd);  // 슬래시 쿨타임 너무 짧아서 테스트용
		}
		else {

			AN_Graduation_projectCharacter* MyChar = GetOwner<AN_Graduation_projectCharacter>();
			MyChar->EndAction();
			UE_LOG(LogTemp, Warning, TEXT("kakao no distance"));

		}
	}
	else {

		AN_Graduation_projectCharacter* MyChar = GetOwner<AN_Graduation_projectCharacter>();
		MyChar->EndAction();
		UE_LOG(LogTemp, Warning, TEXT("kakao No CanUseNomalSkill"));

	}
}

void UPlayerSkillComponent::SpecialSkillPlay(const FString& SkillID)
{
	if (CanUseSpecialSkill == true)
	{
		CanUseSpecialSkill = false;

		FSkillData SkillData;
		if (!UABGameSingleton::Get().GetSkillDataBySkillID(SkillID, SkillData))
		{
			//	UE_LOG(LogTemp, Warning, TEXT("No UABGameSingleton"));

			return;
		}

		//스페셜 스킬
		if (SkillID == "Skill_ShieldGuard") {
			// 방어 스킬 처리 (범위 밖 스킬)
			if (!CanUseSpecialSkill)
			{
				UE_LOG(LogTemp, Warning, TEXT("Defense skill is on cooldown!"));
				return;  // 쿨타임 중이면 방어 스킬 실행하지 않음
			}
			OnDefenseSkill(3.0);  // 방어 스킬 실행
		}
		// 쿨타임 후 방어 스킬 사용 가능하게 설정
		CanUseSpecialSkill = false;  // special 스킬 쿨타임 시작
		FTimerDelegate SpecialCooldownEnd;
		SpecialCooldownEnd.BindUObject(this, &UPlayerSkillComponent::SpecialCooldown);
		SetSkillTimer(SkillData.SkillCoolTime, SpecialCooldownEnd);  // 쿨타임 설정
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
		//UE_LOG(LogTemp, Log, TEXT("Get CharacterOwner"));

		UActionAnimInstance* ActionAnimInstance = Cast<UActionAnimInstance>(AnimInstance);
		if (ActionAnimInstance)
		{
			ActionAnimInstance->PlayAnimation(EffectID);

			// 델리게이트 바인딩 추가
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &UPlayerSkillComponent::EndSkillAnimation);
			ActionAnimInstance->Montage_SetEndDelegate(EndDelegate);
			SkillEffect(EffectID);
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
	FSkillEffectData EffectData;
	if (!UABGameSingleton::Get().GetSkillEffectDataTBySkillID(SkillNameID, EffectData))
	{
		UE_LOG(LogTemp, Warning, TEXT("no Effect: %s"), *SkillNameID);
		return;
	}
	DamageAmount = EffectData.EffectValue01;
	UE_LOG(LogTemp, Warning, TEXT("SkillEffect 실행됨! DamageAmount: %f"), DamageAmount);

}
//void UPlayerSkillComponent::OnHitboxOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	UE_LOG(LogTemp, Log, TEXT("%s"), *(OverlappedComponent->GetName()));
//}
//void UPlayerSkillComponent::SkillAnimation(const FString& EffectID) {
//	if(CurrentSkillID==Skill_Slash)
//
//}