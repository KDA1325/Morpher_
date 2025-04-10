#include "WidgetActor.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

UWidgetActor::UWidgetActor()
{
    PrimaryComponentTick.bCanEverTick = false;

	static ConstructorHelpers::FClassFinder<UUserWidget> HUD(TEXT("WidgetBlueprint'/Game/GUI/HUD_Collection.HUD_Collection_C'"));
	if (HUD.Succeeded())
	{
		HUDClass = HUD.Class;
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
            UE_LOG(LogTemp, Warning, TEXT("HUDWidget Created and Added to Viewport"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("HUDWidget is NULL after CreateWidget"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("HUDClass is NULL!"));
    }
}
