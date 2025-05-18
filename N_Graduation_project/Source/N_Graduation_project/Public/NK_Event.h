#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h" // GetAllActorsOfClass 사용을 위해 필요
#include "UObject/ConstructorHelpers.h" // FObjectFinder 사용을 위해 필요
#include "NK_Event.generated.h"

UCLASS()
class N_GRADUATION_PROJECT_API ANK_Event: public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ANK_Event();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// 콤포넌트 선언
	UPROPERTY(VisibleAnywhere)
		UBoxComponent* BoxComponent;

	//UPROPERTY(BlueprintReadOnly)
	//	bool GetKey = false;

	UPROPERTY(VisibleAnywhere)
		TSubclassOf<AActor> WallBPClass;

	UFUNCTION(BlueprintImplementableEvent)
		void DestroyEvent();

	UFUNCTION()
		void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,
			UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);
};
