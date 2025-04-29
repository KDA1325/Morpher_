#include "MyPlayerStatComponent.h"
#include "N_Graduation_project/N_Graduation_projectCharacter.h"
#include "MyCharacterWidget.h"
#include "WidgetActor.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"

UMyPlayerStatComponent::UMyPlayerStatComponent()
{
	CurrentMana = 10;
	NewMaxHP = 150;
	PastMaxHP = 150;
	CurrentHP = 150;
	PastCurrentHP = 150;
	MonsterName = "PlayerCharacter";
	Change = true;
}

UMyCharacterWidget* UMyPlayerStatComponent::GetHUD() const
{
	if (AActor* Owner = GetOwner())
	{
		if (UWidgetActor* WidgetActor = Owner->FindComponentByClass<UWidgetActor>())
		{
			return WidgetActor->GetHUDWidget();
		}
	}
	return nullptr;
}

void UMyPlayerStatComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UMyPlayerStatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner()))
	{
		OwnerPlayer = Cast<AN_Graduation_projectCharacter>(CharacterOwner);
	}

	UpdateHUD(); // ���� ���� �� HUD �ʱ�ȭ
}

void UMyPlayerStatComponent::SetHP(float NewHP)
{
//	PastCurrentHP = CurrentHP;
	CurrentHP = FMath::Max(0.0f, NewHP);

	UpdateHUD();

	if (CurrentHP == 0)
	{
		OwnerPlayer->OnPlayerDead();
	}
}

void UMyPlayerStatComponent::SetMaxHp(int MaxHp)
{
	//PastMaxHP = NewMaxHP;
	NewMaxHP = MaxHp;
}

void UMyPlayerStatComponent::ApplyDamage(float DamageAmount)
{
	SetHP(CurrentHP - DamageAmount);
}

void UMyPlayerStatComponent::SetMana(int NewMana)
{
	CurrentMana = NewMana;
	UpdateHUD();
}

void UMyPlayerStatComponent::UseMana(int ManaAmount)
{
	if (CurrentMana >= ManaAmount)
	{
		SetMana(CurrentMana - ManaAmount);
	}
}

void UMyPlayerStatComponent::RegenerateMana()
{
	if (CurrentMana < 10)
	{
		SetMana(CurrentMana + 1);
	}
	if (CurrentMana >= 2) {
		Change = true;
	}
}

void UMyPlayerStatComponent::TransformToEntity(FString Name, int HP, int Mana)
{
	if (CurrentMana >= Mana)
	{
		Change = true;
		PastMaxHP = NewMaxHP;
		PastCurrentHP = CurrentHP;
		UE_LOG(LogTemp, Log, TEXT("maxhp %f %f "), PastCurrentHP,PastMaxHP);

	

		if (UMyCharacterWidget* HUD = GetHUD())
		{
			GetWorld()->GetTimerManager().SetTimer(ManaRegenTimerHandle, this, &UMyPlayerStatComponent::RegenerateMana, 4.0f, true);

			MonsterName = Name;
			HUD->SkillName = MonsterName;
			HUD->SetSkillIcon();
			HUD->ChangeIcon(MonsterName);

			HUD->PassedTimeNomal = 0.0f;
			HUD->PassedTimeSpecial = 0.0f;
			HUD->SkillCoolTimeNomal = 0.0f;
			HUD->SkillCoolTimeSpecial = 0.0f;
			HUD->CanNomal = true;
			HUD->CanSpecial = true;

			HUD->UpdateNomalSkillCooldown(0.0f, true, false);
			HUD->UpdateSpecialSkillCooldown(0.0f, false, true);
		}

		
		UE_LOG(LogTemp, Log, TEXT("maxhp2 %f %f %d "), PastCurrentHP, PastMaxHP, NewMaxHP);
		if (PastCurrentHP == PastMaxHP)
		{
			SetMaxHp(HP);
			UseMana(Mana);
			SetHP(HP);
		}
		else
		{
			float NewHP = ((CurrentHP / PastMaxHP) * 100 + 10) * (HP / 100);
			SetMaxHp(HP);
			SetHP(FMath::Min(FMath::CeilToFloat(NewHP), (float)NewMaxHP));
			UseMana(Mana);
		}
	}
	else
	{
		Change = false;
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("마나가 부족합니다")));
	}
}

void UMyPlayerStatComponent::UpdateHUD()
{
	if (UMyCharacterWidget* HUD = GetHUD())
	{
		HUD->UpdateHPBar(CurrentHP, NewMaxHP);
		HUD->UpdateMana(CurrentMana);
		HUD->ChangeIcon(MonsterName);
	}
}

