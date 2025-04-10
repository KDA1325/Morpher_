#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyCharacterWidget.h" // 캐릭터 HUD 위젯
#include "PieMenuWidget.h"
#include "WidgetActor.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class N_GRADUATION_PROJECT_API UWidgetActor : public UActorComponent
{
	GENERATED_BODY()

public:
	UWidgetActor();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> HUDClass;

	UPROPERTY()
	UMyCharacterWidget* HUDWidget;
	
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> PieClass;

	UPROPERTY()
	UPieMenuWidget* PieWidget;

	// 접근용 Getter 함수
	UMyCharacterWidget* GetHUDWidget() const { return HUDWidget; }
};
