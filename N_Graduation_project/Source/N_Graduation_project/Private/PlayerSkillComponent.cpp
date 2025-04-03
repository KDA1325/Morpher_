#include "PlayerSkillComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"

#include "TimerManager.h"
#include "CharacterStateComponent.h" //state
#include "EngineUtils.h"

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

	ACharacter* PlayerCharacter = GetWorld()->GetFirstPlayerController()->GetCharacter();
	APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();

	if (PlayerPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("Slave PlayerPawn"));

		HitBox = PlayerPawn->FindComponentByClass<UBoxComponent>();
		Arrow = PlayerPawn->FindComponentByClass<UArrowComponent>();

		if (HitBox)
		{
			HitBox->SetVisibility(false);
		}

		if (Arrow)
		{
			Arrow->SetVisibility(false);
		}
	}

}
void UPlayerSkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
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
	UE_LOG(LogTemp, Log, TEXT("Kakao NomalCooldown"));
}
void UPlayerSkillComponent::SpecialCooldown()
{
	CanUseSpecialSkill = true;
	//	UE_LOG(LogTemp, Log, TEXT("Defense skill is ready to use again!"));
}

/* 히트박스 관련 */
void UPlayerSkillComponent::SettingHitBox(const FSkillData& SkillData)
{
	if (!OnceHitBox && HitBox)
	{
		// 히트박스 크기 및 위치 설정
		FVector NewBoxExtent = FVector(SkillData.SkillTypeSizeX, SkillData.SkillTypeSizeY, 100);
		HitBox->SetBoxExtent(NewBoxExtent);

		FVector NewLocation = HitBox->GetRelativeLocation();
		NewLocation.X += SkillData.SkillTypeSizeX;
		HitBox->SetRelativeLocation(NewLocation);

		OnceHitBox = true;
	}
}
void UPlayerSkillComponent::OnHitBox(const FSkillData& SkillData)
{
	if (HitBox && Arrow)
	{
		HitBox->SetVisibility(true);
		Arrow->SetVisibility(true);

		// 로그 추가: 활성화된 히트박스와 화살의 상태 확인
	//	UE_LOG(LogTemp, Warning, TEXT("HitBox and Arrow"));
	}
	else
	{
		// HitBox나 Arrow가 없는 경우 로그 추가
		//UE_LOG(LogTemp, Error, TEXT("Failed to find HitBox or Arrow!"));
	}
}
void UPlayerSkillComponent::HideHitBox()
{
	//UE_LOG(LogTemp, Error, TEXT("HideHitBox() called!"));

	if (HitBox && Arrow)
	{
		HitBox->SetVisibility(false);
		Arrow->SetVisibility(false);
		// 로그 추가: 히트박스와 화살이 숨겨졌는지 확인
		//UE_LOG(LogTemp, Warning, TEXT("HitBox and Arrow hidden."));
	}
	else {
		//	UE_LOG(LogTemp, Warning, TEXT("No HitBox and Arrow hidden."));

	}
}

/* 몬스터 거리 측정 관련 */
AActor* UPlayerSkillComponent::FindMonsterTarget() const
{
	if (!GetWorld()) {
		//	UE_LOG(LogTemp, Error, TEXT("NO GetWorld"));
		return nullptr;
	}
	else {
		//			UE_LOG(LogTemp, Error, TEXT("Yes GetWorld"));

		AActor* ClosestMonster = nullptr;
		float MinDistance = FLT_MAX;

		for (TActorIterator<AActor> It(GetWorld()); It; ++It)
		{
			AActor* Actor = *It;
			if (Actor && Actor->ActorHasTag(FName("Monster")))
			{
				float CurrentDistance = GetDistanceTo(Actor);
				if (CurrentDistance < MinDistance)
				{
					MinDistance = CurrentDistance;
					ClosestMonster = Actor;
				}
			}
		}
		return ClosestMonster;  // 가장 가까운 몬스터 반환
	}
}

float UPlayerSkillComponent::GetDistanceTo(const AActor* OtherActor) const
{
	// 현재 액터와 플레이어 간 거리 계산
	return OtherActor ? (GetOwner()->GetActorLocation() - OtherActor->GetActorLocation()).Size() : 0.f;
}

float UPlayerSkillComponent::MeasureDistanceToMonster() const
{
	AActor* MonsterTarget = FindMonsterTarget();
	if (MonsterTarget)
	{
		//UE_LOG(LogTemp, Warning, TEXT("GetDistanceTo(MonsterTarget): %f"), GetDistanceTo(MonsterTarget));
		return GetDistanceTo(MonsterTarget);
	}
	return 0.f;
}

/* 스킬 실행 */
void UPlayerSkillComponent::VisibleHitBox(const FString& SkillID)
{
	//UE_LOG(LogTemp, Warning, TEXT("Yes NomalSkillType"));
//	SkillAnimation("Skill_Slash");

	FSkillData SkillData;
	if (!UABGameSingleton::Get().GetSkillDataBySkillID(SkillID, SkillData))
	{
		//	UE_LOG(LogTemp, Warning, TEXT("No UABGameSingleton"));

		return;
	}

	distance = MeasureDistanceToMonster();
	UE_LOG(LogTemp, Warning, TEXT("Distance to Monster: %f"), distance);

	//	히트박스 처리 (범위 내 스킬)
	if (distance <= SkillData.SkillRange)
	{
		//SkillAnimation(SkillData.SkillNameID);

		UE_LOG(LogTemp, Error, TEXT("distance: %f, SkillRange: %f"), distance, SkillData.SkillRange);

		if (SkillData.SkillTypeShape == EnumSkillTypeShape::Box)
		{
			SettingHitBox(SkillData);  // 히트박스 초기화
			OnHitBox(SkillData);    // 히트박스 활성화
			UE_LOG(LogTemp, Warning, TEXT("kakao On OnHitBox"));

		}
		else if (SkillData.SkillTypeShape == EnumSkillTypeShape::Sphere)
		{
			//	 Sphere 관련 처리 추가
		}
	}
	else {
		FTimerHandle  HitboxEnd;
		GetWorld()->GetTimerManager().SetTimer(HitboxEnd, this, &UPlayerSkillComponent::HideHitBox, SkillData.SkillDuration, false);

	}
}
void UPlayerSkillComponent::NomalSkillPlay(const FString& SkillID)
{
	UE_LOG(LogTemp, Warning, TEXT("kakao On NomalSkillPlay"));

	if (CanUseNomalSkill == true) {

		UE_LOG(LogTemp, Warning, TEXT("kakao On CanUseNomalSkill"));

		FSkillData SkillData;
		if (!UABGameSingleton::Get().GetSkillDataBySkillID(SkillID, SkillData))
		{
			//	UE_LOG(LogTemp, Warning, TEXT("No UABGameSingleton"));

			return;
		}

		CanUseNomalSkill = false;

		//노말 스킬
		if (SkillID == "Skill_Slash") {
			SkillAnimation(SkillID);
			UE_LOG(LogTemp, Warning, TEXT("kakao On SkillSlash"));

		}
		FTimerDelegate NomalCooldownEnd;
		NomalCooldownEnd.BindUObject(this, &UPlayerSkillComponent::NomalCooldown);//바인딩
		SetSkillTimer(SkillData.SkillCoolTime, NomalCooldownEnd);  // 쿨타임 설정
		//SetSkillTimer(3.0f, NomalCooldownEnd);  // 슬래시 쿨타임 너무 짧아서 테스트용
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
	UE_LOG(LogTemp, Warning, TEXT("Skill Animation Ended: %s"), *Montage->GetName());
	AActor* OwnerActor = GetOwner();

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