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
	bool Back_CacheFinalMouseAngle;
	bool first=true;
	float Before_Select = 0.0f;

	void HidePieMenu();
	void ShowDieWidget();
	void ShowPieMenu();
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> HUDClass;

	UPROPERTY()
	UMyCharacterWidget* HUDWidget;
	
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> PieClass;

	UPROPERTY()
	UPieMenuWidget* PieWidget;

	UPROPERTY(EditAnywhere,Category = "UI")
		TSubclassOf<UUserWidget> DieClass;
	

	UPROPERTY(EditAnywhere,Category = "UI")
	UUserWidget* DieWidget;
	

	// 접근용 Getter 함수
	UMyCharacterWidget* GetHUDWidget() const { return HUDWidget; }

	UFUNCTION(BlueprintCallable, Category = "UI")
	UPieMenuWidget* GetPieWidget() const { return PieWidget; }
};
