#include "MyCharacterWidget.h"

void UMyCharacterWidget::NativeConstruct()
{
	Super::NativeConstruct();

	for (int i = 0; i < 10; i++)
	{
		FString Name = FString::Printf(TEXT("mp_slot%d"), i);
		if (UImage* Image = Cast<UImage>(GetWidgetFromName(*Name)))
		{
			ManaTokenImages.Add(Image);
		}
	}

	for (int i = 0; i < 7; i++)
	{
		FString Name = FString::Printf(TEXT("P_icon%d"), i);
		if (UImage* Icon = Cast<UImage>(GetWidgetFromName(*Name)))
		{
			IconImages.Add(Icon);
		}
	}
}

void UMyCharacterWidget::UpdateHPBar(float CurrentHP, float MaxHP)
{
	if (UProgressBar* HealthBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("ProgressBar_55"))))
	{
		HealthBar->SetPercent(CurrentHP / MaxHP);
	}

	if (UTextBlock* HealthText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextBlock_369"))))
	{
		HealthText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), CurrentHP, MaxHP)));
	}
}


void UMyCharacterWidget::UpdateMana(int ManaAmount)
{
	for (int i = 0; i < ManaTokenImages.Num(); i++)
	{
		if (ManaTokenImages[i])
		{
			ManaTokenImages[i]->SetVisibility(i < ManaAmount ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		}
	}
}

void UMyCharacterWidget::ChangeIcon(const FString& MonsterName)
{
	for (UImage* Icon : IconImages)
	{
		if (Icon) Icon->SetVisibility(ESlateVisibility::Hidden);
	}

	TMap<FString, int32> IconMap = {
		{"PlayerCharacter", 0},
		{"WildBoar", 1},
		{"Inpermon", 2},
		{"Freezard", 3},
		{"StoneGolem", 4},
		{"SkeletonWarrior", 5},
		{"SkeletonArcher", 6}
	};

	if (IconMap.Contains(MonsterName) && IconImages.IsValidIndex(IconMap[MonsterName]))
	{
		IconImages[IconMap[MonsterName]]->SetVisibility(ESlateVisibility::Visible);
	}
}
void UMyCharacterWidget::UpdateSkillCooldown( float cooltime, bool nomal, bool special)
{
	CoolTime = cooltime;
	CanNomal = nomal;
	CanSpecial = special;
	UE_LOG(LogTemp, Log, TEXT("toto time: %f"), CoolTime);

}