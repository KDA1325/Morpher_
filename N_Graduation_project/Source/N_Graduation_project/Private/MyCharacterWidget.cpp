#include "MyCharacterWidget.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

void UMyCharacterWidget::NativeConstruct()
{
	Super::NativeConstruct();
	//마나
	for (int i = 0; i < 10; i++)
	{
		FString Name = FString::Printf(TEXT("mp_slot%d"), i);
		if (UImage* Image = Cast<UImage>(GetWidgetFromName(*Name)))
		{
			ManaTokenImages.Add(Image);
		}
	}
	//프로필
	for (int i = 0; i < 7; i++)
	{
		FString Name = FString::Printf(TEXT("P_icon%d"), i);
		if (UImage* Icon = Cast<UImage>(GetWidgetFromName(*Name)))
		{
			IconImages.Add(Icon);
		}
	}
	SkillName = "PlayerCharacter";
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
void UMyCharacterWidget::UpdateSkillCooldown(float cooltime, bool nomal, bool special)
{
	SkillCoolTime = cooltime;
	CanNomal = nomal;
	CanSpecial = special;
	UE_LOG(LogTemp, Log, TEXT("toto widget CoolTime: %f"), SkillCoolTime);
}
void UMyCharacterWidget::SetSkillIcon() {
	UImage* SkillIcon1 = Cast<UImage>(GetWidgetFromName(TEXT("Skill_Icon_1")));
	UImage* SkillIcon2 = Cast<UImage>(GetWidgetFromName(TEXT("Skill_Icon_2")));

	if (SkillIcon1&& SkillIcon2)
	{
		if (SkillName == "PlayerCharacter") {
			UMaterialInterface* BaseMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/UI/Materials/MI_Cooldown.MI_Cooldown"));
			UMaterialInterface* BaseMaterial2 = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/UI/Materials/MI_Cooldown2.MI_Cooldown2"));
			if (BaseMaterial)
			{
				// 다이나믹 머티리얼 인스턴스 생성
				CooldownMID1 = UMaterialInstanceDynamic::Create(BaseMaterial, this);
				CooldownMID2 = UMaterialInstanceDynamic::Create(BaseMaterial2, this);

				// 브러시에 머티리얼 인스턴스 설정
				SkillIcon1->SetBrushFromMaterial(CooldownMID1);
				SkillIcon2->SetBrushFromMaterial(CooldownMID2);

				// 초기화
				PassedTime = 0.f;
			}
		}
		else if (SkillName == "WildBoar") {
			UMaterialInterface* BaseMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/UI/Materials/MI_Cooldown2.MI_Cooldown2"));
			UMaterialInterface* BaseMaterial2 = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/UI/Materials/MI_Cooldown.MI_Cooldown"));
			if (BaseMaterial)
			{
				// 다이나믹 머티리얼 인스턴스 생성
				CooldownMID1 = UMaterialInstanceDynamic::Create(BaseMaterial, this);
				CooldownMID2 = UMaterialInstanceDynamic::Create(BaseMaterial2, this);

				// 브러시에 머티리얼 인스턴스 설정
				SkillIcon1->SetBrushFromMaterial(CooldownMID1);
				SkillIcon2->SetBrushFromMaterial(CooldownMID2);

				// 초기화
				PassedTime = 0.f;
			}
		}
	}
}

void UMyCharacterWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	//UE_LOG(LogTemp, Log, TEXT("NativeTick CanNomal %s"),CanNomal ? TEXT("true") : TEXT("false"));
	SetSkillIcon();

	if (CanNomal && CooldownMID1)
	{
		if (PassedTime < 1.0f)
		{
			PassedTime += InDeltaTime / SkillCoolTime;
			PassedTime = FMath::Clamp(PassedTime, 0.0f, 1.0f);

			CooldownMID1->SetScalarParameterValue(TEXT("percent"), PassedTime - 1.0f);
		}
	}
}
