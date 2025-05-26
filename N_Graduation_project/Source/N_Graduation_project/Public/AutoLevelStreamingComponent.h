// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AutoLevelStreamingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class N_GRADUATION_PROJECT_API UAutoLevelStreamingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAutoLevelStreamingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable,Category="Level Streaming")
		static TArray<FName> GetLoadedStreamingLevelNames(UObject* WorldContextObject);

	//UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Level Streaming")
	//TArray<FName> ConnectedLevels;

	/*UFUNCTION()
	void OnTriggerOverlap(AActor* OverlappedActor,AActor* OtherActor);*/

private:
	// 현재 로드된 레벨 추적용
	//TSet<FName> LoadedLevels; 
};
