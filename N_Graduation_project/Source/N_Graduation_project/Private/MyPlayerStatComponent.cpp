#include "MyPlayerStatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ABEntityData.h" // Entity Data 구조체
#include "ABGameSingleton.h"
#include <N_Graduation_project/N_Graduation_projectCharacter.h>
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

UMyPlayerStatComponent::UMyPlayerStatComponent()
{
	CurrentMana = 10;
	NewMaxHP = 150;
	PastMaxHP = 150;
	CurrentHP = 150;
	MonsterName = "PlayerCharacter";
	//위젯 블루프린트 클래스 찾기
	static ConstructorHelpers::FClassFinder<UUserWidget> HUD(TEXT("WidgetBlueprint'/Game/GUI/HUD_Profile.HUD_Profile_C'"));
	if (HUD.Succeeded())
	{
		HUDClass = HUD.Class; // 위젯 클래스 설정
	}
}

void UMyPlayerStatComponent::InitializeComponent()
{
	Super::InitializeComponent();
	IconChange();
}
void UMyPlayerStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// OwnerPlayer를 AN_Graduation_projectCharacter로 캐스팅하여 설정
	if (ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner())) // GetOwner()는 이 컴포넌트를 소유한 액터를 반환
	{
		OwnerPlayer = Cast<AN_Graduation_projectCharacter>(CharacterOwner); // 캐스팅
	}

	if (HUDClass)
	{
		HUDWidget = CreateWidget(GetWorld()->GetFirstPlayerController(), HUDClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();

			// 위젯에서 마나 토큰 이미지(이름: mp_slot0 ~ mp_slot9)를 배열에 저장
			for (int i = 0; i < 10; i++)
			{
				FString WidgetName = FString::Printf(TEXT("mp_slot%d"), i);
				if (UImage* ManaImage = Cast<UImage>(HUDWidget->GetWidgetFromName(*WidgetName)))
				{
					ManaTokenImages.Add(ManaImage);
				}

			}
			// 위젯에서 아이콘 이미지(이름: P_icon0 ~ P_icon6)를 배열에 저장
			for (int i = 0; i < 7; i++)
			{
				FString IconName = FString::Printf(TEXT("P_icon%d"), i);
				if (UImage* Icon = Cast<UImage>(HUDWidget->GetWidgetFromName(*IconName)))
				{
					IconImages.Add(Icon);
				}
			}
			UpdateHUD();
		}
	}

	// 4초마다 RegenerateMana를 호출하여 마나 회복
	GetWorld()->GetTimerManager().SetTimer(ManaRegenTimerHandle, this, &UMyPlayerStatComponent::RegenerateMana, 4.0f, true);

}
void UMyPlayerStatComponent::RegenerateMana()
{
	if (CurrentMana < 10) {
		SetMana(CurrentMana + 1);
	}
}
void UMyPlayerStatComponent::SetHP(float NewHP)
{
	if (NewHP < 0)
	{
		CurrentHP = 0;

		if (OwnerPlayer)
		{
			OwnerPlayer->OnPlayerDead();
		}
		//OnHPIsZero.Broadcast();
	}
	else
	{
		CurrentHP = NewHP;
		UE_LOG(LogTemp, Log, TEXT("avocado SetHp_CurrentHP: %f"), CurrentHP);
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("Current HP: %0.f, %f"), CurrentHP, NewMaxHP));
	}
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
	UE_LOG(LogTemp, Log, TEXT("avocado CurrentMana: %d"), CurrentMana);

	// 마나 토큰 이미지 업데이트 : CurrentMana 개수만큼 Visible, 나머지는 Hidden
	for (int i = 0; i < ManaTokenImages.Num(); i++)
	{
		if (ManaTokenImages[i])
		{
			ManaTokenImages[i]->SetVisibility(i < CurrentMana ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		}
	}
}

void UMyPlayerStatComponent::UseMana(int ManaAmount)
{
	if (CurrentMana >= ManaAmount)
	{
		SetMana(CurrentMana - ManaAmount);
	}
}

void UMyPlayerStatComponent::TransformToEntity(FString Name, int HP, int Mana)
{
	if (CurrentMana > Mana) 
	{
		MonsterName = Name;
		IconChange();
		if (CurrentHP == PastMaxHP) {
			SetMaxHp(HP);
			UseMana(Mana);
			SetHP(HP);
			UE_LOG(LogTemp, Log, TEXT("avocado %s : CurrentHP: %f,  BeforeMaxHP: %f"), *Name, CurrentHP, NewMaxHP);

		}
		else
		{
			// 변신 전 최대 HP 저장 (BeforeMaxHP)
			//UE_LOG(LogTemp, Log, TEXT("avocado CurrentHP: %f,  BeforeMaxHP: %f, NewMaxHP:%f"), CurrentHP, PastMaxHP, NewMaxHP);
			
				float NewHP = ((CurrentHP / PastMaxHP) * 100 + 10) * (HP / 100);
				UE_LOG(LogTemp, Log, TEXT("avocado CurrentHP / PastMaxHP: %f,  HP / 100: %d, NewHP: %f"), CurrentHP / PastMaxHP, HP / 100, NewHP);
				UE_LOG(LogTemp, Log, TEXT("avocado HP: %f,  Mana: %d"), NewHP, Mana);

				SetMaxHp(HP);

				if (NewHP < NewMaxHP) {
					// 새로운 HP 값을 올림 처리
					NewHP = FMath::CeilToFloat(NewHP);
				}
				else NewHP = NewMaxHP;
				UseMana(Mana);
				SetHP(NewHP);
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("You can not Transform to Entity"));
	}
}

void UMyPlayerStatComponent::UpdateHUD()
{
	//UE_LOG(LogTemp, Log, TEXT("avocado UpdateHUD"));
	if (HUDWidget)
	{
		//UE_LOG(LogTemp, Log, TEXT("avocado Yes HUDWidget"));
		// HUDWidget에서 ProgressBar_55와 TestBlock_369를 찾아 업데이트
		if (UProgressBar* HealthBar = Cast<UProgressBar>(HUDWidget->GetWidgetFromName(TEXT("ProgressBar_55"))))
		{
			HealthBar->SetPercent(CurrentHP / NewMaxHP);
			UE_LOG(LogTemp, Log, TEXT("avocado SetPercent: %f"), CurrentHP / NewMaxHP);

		}
		if (UTextBlock* HealthText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("TextBlock_369"))))
		{
			HealthText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), CurrentHP, NewMaxHP)));
			//UE_LOG(LogTemp, Log, TEXT("avocado Found HealthText, setting text: %.0f / %.0f"), CurrentHP, NewMaxHP);
		}
	}
	else {
		//UE_LOG(LogTemp, Log, TEXT("avocado No HUDWidget"));
	}
}

void UMyPlayerStatComponent::IconChange()
{
	UE_LOG(LogTemp, Error, TEXT("avocado IconChange"));
	// 모든 아이콘을 Hidden으로 설정
	for (UImage* Icon : IconImages)
	{
		if (Icon)
		{
			Icon->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	if (MonsterName == TEXT("PlayerCharacter")) {
		IconImages[0]->SetVisibility(ESlateVisibility::Visible);
		//UE_LOG(LogTemp, Log, TEXT("avocado IconImages1"));
	}
	if (MonsterName == TEXT("WildBoar")) {
		IconImages[1]->SetVisibility(ESlateVisibility::Visible);
		//UE_LOG(LogTemp, Log, TEXT("avocado IconImages2"));
	}
	if (MonsterName == "Inpermon") {
		IconImages[2]->SetVisibility(ESlateVisibility::Visible);
	}
	if (MonsterName == "Freezard") {
		IconImages[3]->SetVisibility(ESlateVisibility::Visible);
	}
	if (MonsterName == "StoneGolem") {
		IconImages[4]->SetVisibility(ESlateVisibility::Visible);
	}
	if (MonsterName == "SkeletonWarrior") {
		IconImages[5]->SetVisibility(ESlateVisibility::Visible);
	}
	if (MonsterName == "SkeletonArcher") {
		IconImages[6]->SetVisibility(ESlateVisibility::Visible);
	}
}