#include "PlayerSkillComponent.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"

UPlayerSkillComponent::UPlayerSkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	IsDefending = false;
	bHitBoxInitialized = false;
}

void UPlayerSkillComponent::SetSkillTimer(float Count, FTimerDelegate End)
{
	if (Count > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, End, Count, false);
	}
}

void UPlayerSkillComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPlayerSkillComponent::OnDefenseSkill(float Count)
{
	if (IsDefending) return; // 중복X

	IsDefending = true;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("DefenseSkill on"));
	}
	// Count초 후 방어 해제
	FTimerDelegate DefenseEnd;
	DefenseEnd.BindUObject(this, &UPlayerSkillComponent::OffDefenseSkill); //Count초 후 OffDefenseSkill호출
	SetSkillTimer(Count, DefenseEnd);
}

void UPlayerSkillComponent::OffDefenseSkill()
{
	IsDefending = false;
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("DefenseSkill off")));
}



/*
		머지 하고 주석풀기
			*/

void UPlayerSkillComponent::SkillType(const FString& SkillID)
{
	CurrentSkillID = SkillID;

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
	//UE_LOG(LogTemp, Error, TEXT("Distance: %f"), distance);

	if (distance <= SkillData.SkillRange)
	{
		// 스킬 타입이 "Box"라면 HitBox와 Arrow를 활성화
		if (SkillData.SkillTypeShape == "Box")
		{
			//UE_LOG(LogTemp, Error, TEXT("Box"));
			// 처음 한 번만 크기 변경
			if (!bHitBoxInitialized)
			{
				FVector NewBoxExtent = FVector(SkillData.SkillTypeSizeX, SkillData.SkillTypeSizeY, 100);
				HitBox->SetBoxExtent(NewBoxExtent);
				bHitBoxInitialized = true; // 한 번만 실행되도록 플래그 설정

				// HitBox 위치도 처음 한 번만 변경
				FVector NewLocation = HitBox->GetRelativeLocation();
				NewLocation.X += SkillData.SkillTypeSizeX;
				HitBox->SetRelativeLocation(NewLocation);
			}
		}
		if (SkillData.SkillType == "HitBox")
		{
			UE_LOG(LogTemp, Error, TEXT("HitBox"));

			if (HitBox && Arrow) // Null 체크
			{
				HitBox->SetVisibility(true);
				Arrow->SetVisibility(true);

			}
			// 일정 시간이 지나면 히트박스 숨기기
			FTimerDelegate HitboxEnd;
			HitboxEnd.BindUObject(this, &UPlayerSkillComponent::HideHitBox);
			//UE_LOG(LogTemp, Error, TEXT(" X: %d, Y: %d, SkillData.SkillDuration _ %f"),  SkillData.SkillTypeSizeX,SkillData.SkillTypeSizeY,SkillData.SkillDuration);
			SetSkillTimer(SkillData.SkillDuration, HitboxEnd);
		}

		// 스킬 타입이 "Projectile"이면 다른 로직 실행
		else if (SkillData.SkillType == "Projectile")
		{
		}
	}
	else if (SkillData.SkillTypeShape == "Sphere")
	{
		//
	}

	else
	{
		//	
	}
}
// 히트박스 숨기는 함수
void UPlayerSkillComponent::HideHitBox()
{
	if (HitBox && Arrow)
	{
		HitBox->SetVisibility(false);
		Arrow->SetVisibility(false);
	}
}
float UPlayerSkillComponent::GetDistanceTo(const AActor* OtherActor) const
{
	// 현 액터와 플레이어이 거리
	return OtherActor ? (GetOwner()->GetActorLocation() - OtherActor->GetActorLocation()).Size() : 0.f;
}