#include "WidgetActor.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h" // <- DPI 스케일을 위해 필요
#include "N_Graduation_project/N_Graduation_projectCharacter.h"
#include "MyPlayerStatComponent.h"

UWidgetActor::UWidgetActor()
{
	PrimaryComponentTick.bCanEverTick = false;

	static ConstructorHelpers::FClassFinder<UUserWidget> HUD(TEXT("WidgetBlueprint'/Game/GUI/HUD_Collection.HUD_Collection_C'"));
	if (HUD.Succeeded())
	{
		HUDClass = HUD.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> Pie(TEXT("WidgetBlueprint'/Game/GUI/HUD_PieMenu.HUD_PieMenu_C'"));
	if (Pie.Succeeded())
	{
		PieClass = Pie.Class;
	}

}

void UWidgetActor::BeginPlay()
{
	Super::BeginPlay();

	if (!HUDWidget && HUDClass)
	{
		HUDWidget = Cast<UMyCharacterWidget>(CreateWidget(GetWorld()->GetFirstPlayerController(), HUDClass));
		if (HUDWidget && !HUDWidget->IsInViewport())
		{
			HUDWidget->AddToViewport();
				UE_LOG(LogTemp, Warning, TEXT("Widget in viewport: %s"), *HUDWidget->GetName());
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay: %s"), *GetOwner()->GetName());


	// PieWidget이 아직 생성되지 않았다면 생성
	if (!PieWidget && PieClass)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		PieWidget = CreateWidget<UPieMenuWidget>(PC, PieClass);
		if (PieWidget)
		{
			PieWidget->AddToViewport(120);
			PieWidget->SetVisibility(ESlateVisibility::Hidden);

		}
	}
}

void UWidgetActor::ShowPieMenu()
{
	if (PieWidget)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		FVector2D ScaledMousePosition;
		// GetMousePositionScaledByDPI()는 이미 DPI를 고려한 좌표를 반환함
		if (PC && UWidgetLayoutLibrary::GetMousePositionScaledByDPI(PC, ScaledMousePosition.X, ScaledMousePosition.Y))
		{
			// 블루프린트에서 계산했던 방식 가져오기
			float CalculatedX = ScaledMousePosition.X - 1100.0f;
			float CalculatedY = ScaledMousePosition.Y - 350.0f;
			FVector2D FinalPosition(CalculatedX, CalculatedY);
			PieWidget->SetPositionInViewport(FinalPosition, false);
			PieWidget->SetVisibility(ESlateVisibility::Visible);
			PieWidget->StandardPosition();
			PC->bShowMouseCursor = true;

			UE_LOG(LogTemp, Log, TEXT("Pie 메뉴 보임, 계산된 위치: %s"), *FinalPosition.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("마우스 위치를 가져오는데 실패했습니다."));
		}
	}
}

void UWidgetActor::HidePieMenu()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	AN_Graduation_projectCharacter* MyChar = GetOwner<AN_Graduation_projectCharacter>();
	UMyPlayerStatComponent* Stat = MyChar->FindComponentByClass<UMyPlayerStatComponent>();
	if (PieWidget)
	{
		if (Stat->Change == true) {
			PieWidget->CacheFinalMouseAngle();
			MyChar->ChangePreset(PieWidget->Monster);
		}

		PieWidget->SetVisibility(ESlateVisibility::Hidden);


		if (PC)
		{
			UWidgetBlueprintLibrary::SetInputMode_GameOnly(PC);
			//PC->bShowMouseCursor = false;
			UE_LOG(LogTemp, Log, TEXT("PieWidget->Monster %s"), *PieWidget->Monster);

		}

		UE_LOG(LogTemp, Log, TEXT("Pie 메뉴 제거 및 게임 입력 모드로 전환"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PieWidget이 nullptr입니다."));
	}
}
