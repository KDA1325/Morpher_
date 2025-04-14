#include "MyCharacterWidget.h"
#include "Components/Image.h"
#include "Components/ScaleBox.h"
#include "Engine/Texture2D.h"

void UMyCharacterWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// 머티리얼 초기화
	UImage* SkillIcon1 = Cast<UImage>(GetWidgetFromName(TEXT("Skill_Icon_1")));
	UImage* SkillIcon2 = Cast<UImage>(GetWidgetFromName(TEXT("Skill_Icon_2")));
	SkillIcon2->SetVisibility(ESlateVisibility::Hidden);

	if (SkillIcon1)
	{
		CooldownMID1 = SkillIcon1->GetDynamicMaterial();
		if (CooldownMID1)
		{
			CooldownMID1->SetScalarParameterValue(TEXT("percent"), 1.0f);
		}
	}
	if (SkillIcon2)
	{
		CooldownMID1 = SkillIcon2->GetDynamicMaterial();
		if (CooldownMID2)
		{
			CooldownMID2->SetScalarParameterValue(TEXT("percent"), 1.0f);

		}
	}

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
	for (int i = 1; i <= 7; i++)
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

void UMyCharacterWidget::UpdateNomalSkillCooldown(float cooltime, bool nomal, bool special)
{
	SkillCoolTimeNomal = cooltime;
	CanNomal = nomal;

	SetSkillIcon();

	// 쿨다운 시작 전, 완전히 찬 상태(1.0)로 초기화
	PassedTimeNomal = 0.0f;
	CooldownMID1->SetScalarParameterValue(TEXT("percent"), 1.0f);
}
void UMyCharacterWidget::UpdateSpecialSkillCooldown(float cooltime, bool nomal, bool special)
{
	SkillCoolTimeSpecial = cooltime;
	CanSpecial = special;

	SetSkillIcon();

	// 쿨다운 시작 전, 완전히 찬 상태(1.0)로 초기화
	PassedTimeSpecial = 0.0f;
}


void UMyCharacterWidget::SetSkillIcon() {
	UImage* SkillIcon1 = Cast<UImage>(GetWidgetFromName(TEXT("Skill_Icon_1")));
	UImage* SkillIcon2 = Cast<UImage>(GetWidgetFromName(TEXT("Skill_Icon_2")));
	SkillIcon2->SetVisibility(ESlateVisibility::Visible);

	if (SkillIcon1 && SkillIcon2)
	{
		if (SkillName == "PlayerCharacter")
		{
			UMaterialInterface* BaseMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/UI/Image/Skill/Skill_Instance/Skill_Slash.Skill_Slash"));
			UMaterialInterface* BaseMaterial2 = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/UI/Materials/MI_Cooldown2.MI_Cooldown2"));

			if (BaseMaterial)
			{
				// 다이나믹 머티리얼 인스턴스 생성
				CooldownMID1 = UMaterialInstanceDynamic::Create(BaseMaterial, this);
				CooldownMID2 = nullptr; // 초기화
				// 브러시에 머티리얼 인스턴스 설정
				SkillIcon1->SetBrushFromMaterial(CooldownMID1);
				//SkillIcon2->SetBrushFromMaterial(CooldownMID2);
				SkillIcon2->SetVisibility(ESlateVisibility::Hidden);

			}

		}
		else if (SkillName == "WildBoar") {
			UMaterialInterface* BaseMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/UI/Image/Skill/Skill_Instance/Skill_bite.Skill_bite"));
			UMaterialInterface* BaseMaterial2 = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/UI/Image/Skill/Skill_Instance/Skill_charge.Skill_charge"));

			if (BaseMaterial)
			{
				// 다이나믹 머티리얼 인스턴스 생성
				CooldownMID1 = UMaterialInstanceDynamic::Create(BaseMaterial, this);
				CooldownMID2 = UMaterialInstanceDynamic::Create(BaseMaterial2, this);
				// 브러시에 머티리얼 인스턴스 설정
				SkillIcon1->SetBrushFromMaterial(CooldownMID1);
				SkillIcon2->SetBrushFromMaterial(CooldownMID2);
			}
		}
	}
}void UMyCharacterWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 노말 스킬 쿨다운 갱신
	if (!CanNomal && SkillCoolTimeNomal > 0.f)
	{
		PassedTimeNomal += InDeltaTime / SkillCoolTimeNomal;

		if (PassedTimeNomal >= 1.0f)
		{
			CanNomal = true;
			PassedTimeNomal = 1.0f;
		}
		PassedTimeNomal = FMath::Clamp(PassedTimeNomal, 0.0f, 1.0f);
		float PercentNomal = FMath::Clamp(PassedTimeNomal - 1.0f, -1.0f, 0.0f);
		CooldownMID1->SetScalarParameterValue(TEXT("percent"), PercentNomal);
	
	}

	// 스페셜 스킬 쿨다운 갱신
	if (!CanSpecial && SkillCoolTimeSpecial > 0.f)
	{
		PassedTimeSpecial += InDeltaTime / SkillCoolTimeSpecial;
		if (PassedTimeSpecial >= 1.0f)
		{
			CanSpecial = true;
			PassedTimeSpecial = 1.0f;
		}
		PassedTimeSpecial = FMath::Clamp(PassedTimeSpecial, 0.0f, 1.0f);
		float PercentSpecial = FMath::Clamp(PassedTimeSpecial - 1.0f, -1.0f, 0.0f);
		CooldownMID2->SetScalarParameterValue(TEXT("percent"), PercentSpecial);
	}
}


void UMyCharacterWidget::OnCollection_Implementation(const FString& DeadMonsterName)
{
	UE_LOG(LogTemp, Warning, TEXT("OnCollection_Implementation 실행됨"));
	OnCollection(DeadMonsterName);
	FString CleanName = DeadMonsterName.TrimStartAndEnd();

	if (UScaleBox* CollectionScaleBoxes = Cast<UScaleBox>(GetWidgetFromName(TEXT("Collection_box")))) {

		if (UImage* CollecterIcon = Cast<UImage>(GetWidgetFromName(TEXT("Image_265"))))
		{
			if (DeadMonsterName == "WildBoar") {
				UTexture2D* NewTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/Image/Icon/멧P.멧P"));
				if (NewTexture)
				{
					CollecterIcon->SetBrushFromTexture(NewTexture);
				}
				if (UTextBlock* NameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextBlock_181"))))
				{
					NameText->SetText(FText::FromString(FString::Printf(TEXT("와일드보어"))));
				}
			}
			else if (DeadMonsterName == "Inpermon") {
				UTexture2D* NewTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/Image/Icon/원P.원P"));
				if (NewTexture)
				{
					CollecterIcon->SetBrushFromTexture(NewTexture);
				}
				if (UTextBlock* NameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextBlock_181"))))
				{
					NameText->SetText(FText::FromString(FString::Printf(TEXT("인페르몽"))));
				}
			}
			else if (DeadMonsterName == "SkeletonWarrior") {
				UTexture2D* NewTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/Image/Icon/검P.검P"));
				if (NewTexture)
				{
					CollecterIcon->SetBrushFromTexture(NewTexture);
				}
				if (UTextBlock* NameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextBlock_181"))))
				{
					NameText->SetText(FText::FromString(FString::Printf(TEXT("스켈레톤 전사"))));
				}
			}
			else if (DeadMonsterName == "StoneGolem") {
				UTexture2D* NewTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/Image/Icon/골P.골P"));
				if (NewTexture)
				{
					CollecterIcon->SetBrushFromTexture(NewTexture);
				}
				if (UTextBlock* NameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextBlock_181"))))
				{
					NameText->SetText(FText::FromString(FString::Printf(TEXT("스톤 골렘"))));
				}
			}
			else if (DeadMonsterName == "SkeletonArcher") {
				UTexture2D* NewTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/Image/Icon/활P.활P"));
				if (NewTexture)
				{
					CollecterIcon->SetBrushFromTexture(NewTexture);
				}
				if (UTextBlock* NameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextBlock_181"))))
				{
					NameText->SetText(FText::FromString(FString::Printf(TEXT("스켈레톤 궁수"))));
				}
			}
			else if (DeadMonsterName == "Freezard") {
				UTexture2D* NewTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/Image/Icon/도P.도P"));
				if (NewTexture)
				{
					CollecterIcon->SetBrushFromTexture(NewTexture);
				}
				if (UTextBlock* NameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextBlock_181"))))
				{
					NameText->SetText(FText::FromString(FString::Printf(TEXT("프리자드"))));
				}
			}
		}
	}
}
