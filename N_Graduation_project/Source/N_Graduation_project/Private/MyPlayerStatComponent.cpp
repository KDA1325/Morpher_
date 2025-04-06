#include "MyPlayerStatComponent.h"
#include "N_Graduation_project/N_Graduation_projectCharacter.h"
#include "MyCharacterWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"

UMyPlayerStatComponent::UMyPlayerStatComponent()
{
	CurrentMana = 10;
	NewMaxHP = 150;
	PastMaxHP = 150;
	CurrentHP = 150;
	MonsterName = "PlayerCharacter";

	static ConstructorHelpers::FClassFinder<UUserWidget> HUD(TEXT("WidgetBlueprint'/Game/GUI/HUD_Collection.HUD_Collection_C'"));
	if (HUD.Succeeded())
	{
		HUDClass = HUD.Class;
	}
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

	if (HUDClass)
	{
		HUDWidget = Cast<UMyCharacterWidget>(CreateWidget(GetWorld()->GetFirstPlayerController(), HUDClass));
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
			UpdateHUD();
		}
	}
}

void UMyPlayerStatComponent::SetHP(float NewHP)
{
	CurrentHP = FMath::Max(0.0f, NewHP);
	UpdateHUD();
}

void UMyPlayerStatComponent::SetMaxHp(int MaxHp)
{
	PastMaxHP = NewMaxHP;
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
}

void UMyPlayerStatComponent::TransformToEntity(FString Name, int HP, int Mana)
{
	if (CurrentMana > Mana)
	{
		GetWorld()->GetTimerManager().SetTimer(ManaRegenTimerHandle, this, &UMyPlayerStatComponent::RegenerateMana, 4.0f, true);

		MonsterName = Name;
		HUDWidget-> SkillName=MonsterName;//스킬이미지
		if (HUDWidget) HUDWidget->ChangeIcon(MonsterName);

		if (CurrentHP == PastMaxHP)
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
		UE_LOG(LogTemp, Error, TEXT("You can not Transform to Entity"));
	}
}

void UMyPlayerStatComponent::UpdateHUD()
{
	if (HUDWidget)
	{
		HUDWidget->UpdateHPBar(CurrentHP, NewMaxHP);
		HUDWidget->UpdateMana(CurrentMana);
		HUDWidget->ChangeIcon(MonsterName);
	}
}
