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

void UPlayerSkillComponent::SetSkillTimer(float Count, FTimerDelegate End)
{
	if (GetWorld() && Count > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, End, Count, false);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SetSkillTimer: GetWorld() is null"));
	}
}

void UPlayerSkillComponent::NomalCooldown()
{
	CanUseNomalSkill = true;

	auto StatComponent = GetOwner()->FindComponentByClass<UWidgetActor>();
	if (StatComponent && StatComponent->HUDWidget)
	{
		StatComponent->HUDWidget->CanNomal = true;
		UE_LOG(LogTemp, Log, TEXT("hum PSNomalT CanSpecial, CanNomal: %s %s"),
			StatComponent->HUDWidget->CanSpecial ? TEXT("true") : TEXT("false"),
			StatComponent->HUDWidget->CanNomal ? TEXT("true") : TEXT("false"));
	}

	UE_LOG(LogTemp, Log, TEXT("Kakao NomalCooldown"));
}
void UPlayerSkillComponent::SpecialCooldown()
{
	CanUseSpecialSkill = true;
	auto StatComponent = GetOwner()->FindComponentByClass<UWidgetActor>();
	if (StatComponent && StatComponent->HUDWidget)
	{
		StatComponent->HUDWidget->CanSpecial = true;
		UE_LOG(LogTemp, Log, TEXT("hum PSSpecialT CanSpecial, CanNomal: %s %s"),
			StatComponent->HUDWidget->CanSpecial ? TEXT("true") : TEXT("false"),
			StatComponent->HUDWidget->CanNomal ? TEXT("true") : TEXT("false"));
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
	////정면방향 몬스터 거리
	//if (!GetWorld()) return nullptr;

	//AActor* ClosestMonster = nullptr;//몬스터를 담을 변수
	//float MinDistance = FLT_MAX;//현재까지 찾은 몬스터 중 가장 가까운 거리
	//float MaxDotProduct = -1.0f; // 정면(1에 가까울수록 정면 방향)

	//AActor* Owner = GetOwner();
	//if (!Owner) return nullptr;

	//FVector PlayerLocation = Owner->GetActorLocation();
	//FVector PlayerForward = Owner->GetActorForwardVector(); // 플레이어 정면 방향

	//for (TActorIterator<AActor> It(GetWorld()); It; ++It) //월드에 존재하는 모든 AActor를 순회하며 Monster 태그 확인
	//{
	//	AActor* Actor = *It;
	//	if (Actor && Actor->ActorHasTag(FName("Monster")))
	//	{
	//		FVector ToMonster = (Actor->GetActorLocation() - PlayerLocation).GetSafeNormal();
	//		float DotProduct = FVector::DotProduct(PlayerForward, ToMonster); // 정면과의 유사도 계산

	//		float CurrentDistance = FVector::Dist(PlayerLocation, Actor->GetActorLocation());

	//		// 정면 방향이고, 기존보다 가까운 몬스터를 선택
	//		if (DotProduct > MaxDotProduct || (DotProduct == MaxDotProduct && CurrentDistance < MinDistance))
	//		{
	//			MaxDotProduct = DotProduct;
	//			MinDistance = CurrentDistance;
	//			ClosestMonster = Actor;
	//		}
	//	}
	//}

	//return ClosestMonster;
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
	UE_LOG(LogTemp, Warning, TEXT("Kakao Distance to Monster: %f"), distance);
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
				UE_LOG(LogTemp, Log, TEXT("hum PS CanSpecial, CanNomal: %s %s"),
					StatComponent->HUDWidget->CanSpecial ? TEXT("true") : TEXT("false"),
					StatComponent->HUDWidget->CanNomal ? TEXT("true") : TEXT("false"));
				UE_LOG(LogTemp, Log, TEXT("StatComponent: %p, HUDWidget: %p"), StatComponent, StatComponent ? StatComponent->HUDWidget : nullptr);
			}
			SkillAnimation(SkillID);
			UE_LOG(LogTemp, Warning, TEXT("Playing %s"), *SkillID);
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
		//	MyChar->EndAction();
			UE_LOG(LogTemp, Warning, TEXT("kakao no distance"));

		}
	}
	else {

		AN_Graduation_projectCharacter* MyChar = GetOwner<AN_Graduation_projectCharacter>();
		//MyChar->EndAction();
		UE_LOG(LogTemp, Warning, TEXT("kakao No CanUseNomalSkill"));

	}
}

void UPlayerSkillComponent::SpecialSkillPlay(const FString& SkillID)
{
	distance = MeasureDistanceToMonster();
	if (CanUseSpecialSkill == true) {

		FSkillData SkillData;
		if (!UABGameSingleton::Get().GetSkillDataBySkillID(SkillID, SkillData)) return;

		if (distance <= SkillData.SkillRange)
		{
			AN_Graduation_projectCharacter* MyChar = GetOwner<AN_Graduation_projectCharacter>();
			MyChar->StartAction();

			VisibleShapeBox(SkillID);
			//스킬 쿨타임
			CanUseNomalSkill = false;
			auto StatComponent = GetOwner()->FindComponentByClass<UWidgetActor>();
			if (StatComponent && StatComponent->HUDWidget)
			{
				StatComponent->HUDWidget->UpdateSkillCooldown(SkillData.SkillCoolTime, CanUseNomalSkill, CanUseSpecialSkill);
				StatComponent->HUDWidget->CanSpecial = false;
				UE_LOG(LogTemp, Log, TEXT("StatComponent: %p, HUDWidget: %p"), StatComponent, StatComponent ? StatComponent->HUDWidget : nullptr);
			}
			SkillAnimation(SkillID);
			UE_LOG(LogTemp, Warning, TEXT("Playing %s"), *SkillID);


			if (SkillID == "Skill_Charge")
			{
				StoredDashDirection = MyChar->GetActorForwardVector().GetSafeNormal();
				DrawChargePath(); // 돌진 경로 표시

				float PrepTime = 1.0f;

				// 타이머 바인딩을 위한 델리게이트 생성
				FTimerDelegate DashDelegate;
				DashDelegate.BindUObject(this, &UPlayerSkillComponent::ExecuteChargeDash, SkillData.SkillRange);

				SetSkillTimer(PrepTime, DashDelegate);
			}

			//쿨타임
			FTimerDelegate SpecialCooldownEnd;
			SpecialCooldownEnd.BindUObject(this, &UPlayerSkillComponent::SpecialCooldown);//바인딩
			SetSkillTimer(SkillData.SkillCoolTime, SpecialCooldownEnd);  // 쿨타임 설정
			//SetSkillTimer(3.0f, NomalCooldownEnd);  // 슬래시 쿨타임 너무 짧아서 테스트용
		}
		else {

			AN_Graduation_projectCharacter* MyChar = GetOwner<AN_Graduation_projectCharacter>();
			//MyChar->EndAction();
			UE_LOG(LogTemp, Warning, TEXT("kakao no distance"));
		}
	}
	else {
		AN_Graduation_projectCharacter* MyChar = GetOwner<AN_Graduation_projectCharacter>();
		//MyChar->EndAction();
		UE_LOG(LogTemp, Warning, TEXT("kakao No CanUseNomalSkill"));
	}
}

void UPlayerSkillComponent::DrawChargePath()
{
	// 예시: 디버그 선을 이용해 경로 표시
	ACharacter* MyChar = Cast<ACharacter>(GetOwner());
	if (!MyChar || !GetWorld()) return;

	FVector StartLocation = MyChar->GetActorLocation();
	FVector EndLocation = StartLocation + MyChar->GetActorForwardVector() * 600.f;
	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 1.5f, 0, 5.f);
	UE_LOG(LogTemp, Warning, TEXT("DrawChargePath: Charge path drawn"));

}
void UPlayerSkillComponent::ExecuteChargeDash(float Chargedistance)
{
	// 준비 시간 동안 저장된 dash 방향을 사용
	ACharacter* MyChar = Cast<ACharacter>(GetOwner());
	if (!MyChar) return;

	FVector DashDirection = StoredDashDirection;
	float DashSpeed = 2000.0f;

	MyChar->LaunchCharacter(DashDirection * DashSpeed, true, true);
	UE_LOG(LogTemp, Warning, TEXT("ExecuteChargeDash: Dashing"));
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
	TArray<FSkillEffectData> EffectData;
	if (!UABGameSingleton::Get().GetSkillEffectDataBySkillID(SkillNameID, EffectData))
	{
		UE_LOG(LogTemp, Warning, TEXT("no Effect: %s"), *SkillNameID);
		return;
	}

	DamageAmount = EffectData[0].EffectValue01;
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