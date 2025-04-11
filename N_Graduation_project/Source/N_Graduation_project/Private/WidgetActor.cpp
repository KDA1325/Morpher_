#include "WidgetActor.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

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

    UE_LOG(LogTemp, Warning, TEXT("UWidgetActor::BeginPlay Called"));

    if (HUDClass)
    {
        HUDWidget = Cast<UMyCharacterWidget>(CreateWidget(GetWorld()->GetFirstPlayerController(), HUDClass));
        if (HUDWidget)
        {
            HUDWidget->AddToViewport();
        }
    }
    
    if (PieClass)
    {
        PieWidget = Cast<UPieMenuWidget>(CreateWidget(GetWorld()->GetFirstPlayerController(), PieClass));
        if (PieWidget)
        {
            PieWidget->AddToViewport();
            PieWidget->SetVisibility(ESlateVisibility::Hidden);
        }
    }

}
