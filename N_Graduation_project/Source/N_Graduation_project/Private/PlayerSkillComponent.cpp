#include "PlayerSkillComponent.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "CharacterStateComponent.h" //state

UPlayerSkillComponent::UPlayerSkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	IsDefending = false;
	OnceHitBox = false;
	CanUseHitBoxSkill = true;
	CanUseDefenseSkill = true;
}

void UPlayerSkillComponent::BeginPlay()
{
	Super::BeginPlay();
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

void UPlayerSkillComponent::ResetDefenseSkillCooldown()
{
	CanUseDefenseSkill = true;
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
		FTimerDelegate HitboxEnd;
		HitboxEnd.BindUObject(this, &UPlayerSkillComponent::HideHitBox);
		SetSkillTimer(SkillData.SkillDuration, HitboxEnd);
	}
	else
	{
		// HitBox나 Arrow가 없는 경우 로그 추가
		UE_LOG(LogTemp, Error, TEXT("Failed to find HitBox or Arrow!"));
	}
}

void UPlayerSkillComponent::SkillType(const FString& SkillID)
{
	// 스킬 데이터 가져오기
	FSkillData SkillData;
	if (!UABGameSingleton::Get().GetSkillDataBySkillID(SkillID, SkillData))
	{
		return;
	}

	// 플레이어와의 거리 계산
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController) return;

	APawn* PlayerPawn = PlayerController->GetPawn();
	if (!PlayerPawn) return;

	distance = GetDistanceTo(PlayerPawn);

	// 히트박스 처리 (범위 내 스킬)
	if (distance <= SkillData.SkillRange)
	{
		UE_LOG(LogTemp, Error, TEXT("distance: %f"), distance);

		if (SkillData.SkillTypeShape == "Box")
		{
			SettingHitBox(SkillData);  // 히트박스 초기화
			OnHitBox(SkillData);    // 히트박스 활성화
		}
		else if (SkillData.SkillTypeShape == "Sphere")
		{
			// Sphere 관련 처리 추가
		}
	}
	if (SkillData.SkillID=="Skill_ShieldGuard") {
		// 방어 스킬 처리 (범위 밖 스킬)
		if (!CanUseDefenseSkill)
		{
			UE_LOG(LogTemp, Warning, TEXT("Defense skill is on cooldown!"));
			return;  // 쿨타임 중이면 방어 스킬 실행하지 않음
		}

		OnDefenseSkill(3.0);  // 방어 스킬 실행

		// 쿨타임 후 방어 스킬 사용 가능하게 설정
		FTimerDelegate DefenseCooldownEnd;
		DefenseCooldownEnd.BindUObject(this, &UPlayerSkillComponent::ResetDefenseSkillCooldown);
		SetSkillTimer(SkillData.SkillCoolTime, DefenseCooldownEnd);  // 쿨타임 설정
		CanUseDefenseSkill = false;  // 방어 스킬 쿨타임 시작
	}
}

void UPlayerSkillComponent::HideHitBox()
{
	if (HitBox && Arrow)
	{
		HitBox->SetVisibility(false);
		Arrow->SetVisibility(false);
		// 로그 추가: 히트박스와 화살이 숨겨졌는지 확인
		UE_LOG(LogTemp, Warning, TEXT("HitBox and Arrow hidden."));
	}
}

float UPlayerSkillComponent::GetDistanceTo(const AActor* OtherActor) const
{
	// 현재 액터와 플레이어 간 거리 계산
	return OtherActor ? (GetOwner()->GetActorLocation() - OtherActor->GetActorLocation()).Size() : 0.f;
}

void UPlayerSkillComponent::SkillAnimation(const FString& EffectID) {
	//if(CurrentSkillID==Skill_Slash)
	
}