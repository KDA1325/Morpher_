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
	UE_LOG(LogTemp, Log, TEXT("Defense skill is ready to use again!"));
}
void UPlayerSkillComponent::SpecialCooldown()
{
	CanUseSpecialSkill = true;
	UE_LOG(LogTemp, Log, TEXT("Defense skill is ready to use again!"));
}

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
		UE_LOG(LogTemp, Warning, TEXT("HitBox and Arrow"));
		// 일정 시간이 지나면 히트박스 숨기기
//		FTimerHandle  HitboxEnd;

		//FTimerDelegate HitboxEnd;
		/*HitboxEnd.BindUObject(this, &UPlayerSkillComponent::HideHitBox);
		SetSkillTimer(SkillData.SkillDuration, HitboxEnd);
		UE_LOG(LogTemp, Warning, TEXT("SkillDuration: %f"), SkillData.SkillDuration);*/

		// 타이머 시작: 일정 시간이 지나면 HideHitBox 호출
		//GetWorld()->GetTimerManager().SetTimer(HitboxEnd, this, &UPlayerSkillComponent::HideHitBox, SkillData.SkillDuration, false);
		//UE_LOG(LogTemp, Warning, TEXT("SkillDuration: %f"), SkillData.SkillDuration);
	}
	else
	{
		// HitBox나 Arrow가 없는 경우 로그 추가
		UE_LOG(LogTemp, Error, TEXT("Failed to find HitBox or Arrow!"));
	}
}
void UPlayerSkillComponent::HideHitBox()
{
	UE_LOG(LogTemp, Error, TEXT("HideHitBox() called!"));

	if (HitBox && Arrow)
	{
		HitBox->SetVisibility(false);
		Arrow->SetVisibility(false);
		// 로그 추가: 히트박스와 화살이 숨겨졌는지 확인
		UE_LOG(LogTemp, Warning, TEXT("HitBox and Arrow hidden."));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("No HitBox and Arrow hidden."));

	}
}

//void UPlayerSkillComponent::SkillType(const FString& SkillID)
//{
//	// 스킬 데이터 가져오기
//	FSkillData SkillData;
//	if (!UABGameSingleton::Get().GetSkillDataBySkillID(SkillID, SkillData))
//	{
//		return;
//	}
//
//	// 플레이어와의 거리 계산
//	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
//	if (!PlayerController) return;
//
//	APawn* PlayerPawn = PlayerController->GetPawn();
//	if (!PlayerPawn) return;
//
//	distance = GetDistanceTo(PlayerPawn);
//
//	// 히트박스 처리 (범위 내 스킬)
//	if (distance <= SkillData.SkillRange)
//	{
//		UE_LOG(LogTemp, Error, TEXT("distance: %f"), distance);
//
//		if (SkillData.SkillTypeShape == "Box")
//		{
//			SettingHitBox(SkillData);  // 히트박스 초기화
//			OnHitBox(SkillData);    // 히트박스 활성화
//		}
//		else if (SkillData.SkillTypeShape == "Sphere")
//		{
//			// Sphere 관련 처리 추가
//		}
//	}
//	if (SkillData.SkillID=="Skill_ShieldGuard") {
//		// 방어 스킬 처리 (범위 밖 스킬)
//		if (!CanUseDefenseSkill)
//		{
//			UE_LOG(LogTemp, Warning, TEXT("Defense skill is on cooldown!"));
//			return;  // 쿨타임 중이면 방어 스킬 실행하지 않음
//		}
//
//		OnDefenseSkill(3.0);  // 방어 스킬 실행
//
//		// 쿨타임 후 방어 스킬 사용 가능하게 설정
//		FTimerDelegate DefenseCooldownEnd;
//		DefenseCooldownEnd.BindUObject(this, &UPlayerSkillComponent::ResetDefenseSkillCooldown);
//		SetSkillTimer(SkillData.SkillCoolTime, DefenseCooldownEnd);  // 쿨타임 설정
//		CanUseDefenseSkill = false;  // 방어 스킬 쿨타임 시작
//	}
//}

AActor* UPlayerSkillComponent::FindMonsterTarget()
{
	if (!GetWorld()) {
		UE_LOG(LogTemp, Error, TEXT("NO GetWorld"));
		return nullptr;
	}
	else {
		//	UE_LOG(LogTemp, Error, TEXT("Yes GetWorld"));

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

void UPlayerSkillComponent::NomalSkillType(const FString& SkillID)
{
	FSkillData SkillData;
	if (!UABGameSingleton::Get().GetSkillDataBySkillID(SkillID, SkillData))
	{
		return;
	}

	AActor* MonsterTarget = FindMonsterTarget();  // 몬스터 찾기
	if (!MonsterTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Monster found!"));
		return;
	}

	distance = GetDistanceTo(MonsterTarget);  // 몬스터와 거리 계산
	//UE_LOG(LogTemp, Warning, TEXT("Distance to Monster: %f"), distance);


	//	히트박스 처리 (범위 내 스킬)
	if (distance <= SkillData.SkillRange)
	{
		//SkillAnimation(SkillData.SkillID);

		UE_LOG(LogTemp, Error, TEXT("distance: %f, SkillRange: %d"), distance, SkillData.SkillRange);

		if (SkillData.SkillTypeShape == "Box")
		{
			SettingHitBox(SkillData);  // 히트박스 초기화
			OnHitBox(SkillData);    // 히트박스 활성화

			CanUseNomalSkill = false;  // 방어 스킬 쿨타임 시작
			FTimerDelegate NomalCooldownEnd;
			NomalCooldownEnd.BindUObject(this, &UPlayerSkillComponent::NomalCooldown);//바인딩
			SetSkillTimer(SkillData.SkillCoolTime, NomalCooldownEnd);  // 쿨타임 설정
		}
		else if (SkillData.SkillTypeShape == "Sphere")
		{
			//	 Sphere 관련 처리 추가
		}
	}
	else {
		FTimerHandle  HitboxEnd;
		GetWorld()->GetTimerManager().SetTimer(HitboxEnd, this, &UPlayerSkillComponent::HideHitBox, SkillData.SkillDuration, false);

	}
}
void UPlayerSkillComponent::SpecialSkillType(const FString& SkillID)
{
	FSkillData SkillData;
	if (!UABGameSingleton::Get().GetSkillDataBySkillID(SkillID, SkillData))
	{
		return;
	}


	AActor* MonsterTarget = FindMonsterTarget();  // 몬스터 찾기
	if (!MonsterTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Monster found!"));
		return;
	}

	distance = GetDistanceTo(MonsterTarget);  // 몬스터와 거리 계산
	UE_LOG(LogTemp, Warning, TEXT("Distance to Monster: %f"), distance);


	//	히트박스 처리 (범위 내 스킬)
	if (distance <= SkillData.SkillRange)
	{
		//SkillAnimation(SkillData.SkillID);

		UE_LOG(LogTemp, Error, TEXT("distance: %f"), distance);

		if (SkillData.SkillTypeShape == "Box")
		{
			SettingHitBox(SkillData);  // 히트박스 초기화
			OnHitBox(SkillData);    // 히트박스 활성화

			CanUseNomalSkill = false;  // 방어 스킬 쿨타임 시작
			FTimerDelegate NomalCooldownEnd;
			NomalCooldownEnd.BindUObject(this, &UPlayerSkillComponent::NomalCooldown);//바인딩
			SetSkillTimer(SkillData.SkillCoolTime, NomalCooldownEnd);  // 쿨타임 설정
		}
		else if (SkillData.SkillTypeShape == "Sphere")
		{
			//	 Sphere 관련 처리 추가
		}
		//}
		if (SkillData.SkillID == "Skill_ShieldGuard") {
			// 방어 스킬 처리 (범위 밖 스킬)
			if (!CanUseSpecialSkill)
			{
				UE_LOG(LogTemp, Warning, TEXT("Defense skill is on cooldown!"));
				return;  // 쿨타임 중이면 방어 스킬 실행하지 않음
			}
			OnDefenseSkill(3.0);  // 방어 스킬 실행

			// 쿨타임 후 방어 스킬 사용 가능하게 설정
			CanUseSpecialSkill = false;  // 방어 스킬 쿨타임 시작
			FTimerDelegate SpecialCooldownEnd;
			SpecialCooldownEnd.BindUObject(this, &UPlayerSkillComponent::SpecialCooldown);
			SetSkillTimer(SkillData.SkillCoolTime, SpecialCooldownEnd);  // 쿨타임 설정
		}
	}
	else {
		FTimerHandle  HitboxEnd;
		GetWorld()->GetTimerManager().SetTimer(HitboxEnd, this, &UPlayerSkillComponent::HideHitBox, SkillData.SkillDuration, false);
	}
}

float UPlayerSkillComponent::GetDistanceTo(const AActor* OtherActor) const
{
	// 현재 액터와 플레이어 간 거리 계산
	return OtherActor ? (GetOwner()->GetActorLocation() - OtherActor->GetActorLocation()).Size() : 0.f;
}

void UPlayerSkillComponent::SkillAnimation(const FString& EffectID)
{
	UE_LOG(LogTemp, Warning, TEXT("On SkillAnimation"));


	// GetOwner()로 AActor(혹은 ACharacter) 가져오기
	AActor* OwnerActor = GetOwner();
	if (OwnerActor && OwnerActor->IsA<ACharacter>())
	{
		// ACharacter에서 AnimInstance 가져오기
		ACharacter* CharacterOwner = Cast<ACharacter>(OwnerActor);
		UAnimInstance* AnimInstance = CharacterOwner->GetMesh()->GetAnimInstance();
		UE_LOG(LogTemp, Log, TEXT("Get CharacterOwner"));

		//if (AnimInstance)
		//{
		//	// AnimInstance의 실제 클래스 출력
		//	UE_LOG(LogTemp, Warning, TEXT("AnimInstance Class: %s"), *AnimInstance->GetClass()->GetName());
		//}

		UActionAnimInstance* ActionAnimInstance = Cast<UActionAnimInstance>(AnimInstance);
		if (ActionAnimInstance)
		{
			ActionAnimInstance->PlayAnimation(EffectID);

		}

	}
}
void UPlayerSkillComponent::SkillEffect(const FString& SkillNameID)
{
	FSkillEffectData EffectData;
	if (!UABGameSingleton::Get().GetSkillEffectDataTBySkillID(SkillNameID, EffectData))
	{
		UE_LOG(LogTemp, Warning, TEXT("no Effect: %s"), *SkillNameID);
		return;
	}

	float DamageAmount = EffectData.EffectValue01;
	UE_LOG(LogTemp, Warning, TEXT("SkillEffect 실행됨! DamageAmount: %f"), DamageAmount);




}

//void UPlayerSkillComponent::SkillAnimation(const FString& EffectID) {
//	if(CurrentSkillID==Skill_Slash)
//
//}