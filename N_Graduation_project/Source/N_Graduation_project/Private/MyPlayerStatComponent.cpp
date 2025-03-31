#include "MyPlayerStatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ABEntityData.h" // Entity Data 구조체
#include "ABGameSingleton.h"
#include <N_Graduation_project/N_Graduation_projectCharacter.h>
#include "Blueprint/UserWidget.h"


UMyPlayerStatComponent::UMyPlayerStatComponent()
{
	TransManaCost = 0; //데이터테이블 머지 후 뺴기
	NewMaxHP = 150;
	PastMaxHP = 150;
	CurrentHP = 150;
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
}

void UMyPlayerStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// OwnerPlayer를 AN_Graduation_projectCharacter로 캐스팅하여 설정
	if (ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner())) // GetOwner()는 이 컴포넌트를 소유한 액터를 반환
	{
		OwnerPlayer = Cast<AN_Graduation_projectCharacter>(CharacterOwner); // 캐스팅
	}

	HUDWidget = CreateWidget(GetWorld()->GetFirstPlayerController(), HUDClass);
	HUDWidget->AddToViewport();

	UpdateHUD();
}

void UMyPlayerStatComponent::SetHP(int NewHP)
{
	if (NewHP <= 0)
	{
		CurrentHP = 0;
		//OnHPIsZero.Broadcast();
	}
	else
	{
		CurrentHP = NewHP;
		UE_LOG(LogTemp, Log, TEXT("== CurrentHP: %f"), CurrentHP);	
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("Current HP: %f"), CurrentHP));
		UpdateHUD();
	}
//	OnHPChanged.Broadcast();
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
	//CurrentMana = FMath::Clamp(NewMana, 0, CurrentStatData.TransManaCost);
	CurrentMana = FMath::Clamp(NewMana, 0, TransManaCost);
//	OnManaChangedDelegate.Broadcast();
}

void UMyPlayerStatComponent::UseMana(int ManaAmount)
{
	if (CurrentMana >= ManaAmount)
	{
		SetMana(CurrentMana - ManaAmount);
	}
}

void UMyPlayerStatComponent::TransformToEntity(int HP, int Mana)
{
	SetMaxHp(HP);

	if (CurrentHP == PastMaxHP) {
		UseMana(Mana);
		SetHP(HP);
		UE_LOG(LogTemp, Log, TEXT("== CurrentHP: %f,  BeforeMaxHP: %f"), CurrentHP, PastMaxHP);

	}
	else {
		// 변신 전 최대 HP 저장 (BeforeMaxHP)
		UE_LOG(LogTemp, Log, TEXT("!= CurrentHP: %f,  BeforeMaxHP: %f, NewMaxHP:%d"), CurrentHP, PastMaxHP, NewMaxHP);
		float NewHP = ((CurrentHP / PastMaxHP) * 100 + 10) * (HP / 100);
		UE_LOG(LogTemp, Log, TEXT("!= CurrentHP / PastMaxHP: %f,  HP / 100: %d, NewHP: %f"), CurrentHP / PastMaxHP, HP / 100, NewHP);

		if (NewHP < NewMaxHP) {
			// 새로운 HP 값을 올림 처리
			NewHP = FMath::CeilToFloat(NewHP);
		}
		else NewHP = NewMaxHP;
		
		UseMana(Mana);
		SetHP(NewHP);

		UE_LOG(LogTemp, Log, TEXT("HP: %f,  Mana: %d"), NewHP, Mana);
	}
}

//4초에 1개 추가, 데이터 들어오면 추가 구현
bool UMyPlayerStatComponent::CanTransform(int ManaCost) const
{
	return CurrentMana >= ManaCost;
}

void UMyPlayerStatComponent::UpdateHUD()
{
	UE_LOG(LogTemp, Log, TEXT("avocado UpdateHUD"));
	if (HUDWidget)
	{
		UE_LOG(LogTemp, Log, TEXT("avocado Yes HUDWidget"));
		// HUDWidget에서 ProgressBar_55와 TestBlock_369를 찾아 업데이트
		if (UProgressBar* HealthBar = Cast<UProgressBar>(HUDWidget->GetWidgetFromName(TEXT("ProgressBar_55"))))
		{
			HealthBar->SetPercent(CurrentHP / NewMaxHP);
			UE_LOG(LogTemp, Log, TEXT("avocado SetPercent: %f"),CurrentHP / NewMaxHP);

		}
		if (UTextBlock* HealthText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("TextBlock_369"))))
		{
			HealthText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), CurrentHP, PastMaxHP)));
			UE_LOG(LogTemp, Log, TEXT("avocado Found HealthText, setting text: %.0f / %.0f"), CurrentHP, PastMaxHP);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("avocado Could not find widget named 'TestBlock_369'"));
		}
	}
	else{
		UE_LOG(LogTemp, Log, TEXT("avocado No HUDWidget"));
	}
}