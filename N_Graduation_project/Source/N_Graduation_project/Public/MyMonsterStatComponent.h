
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EntityWidget.h"

#include "MyMonsterStatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class N_GRADUATION_PROJECT_API UMyMonsterStatComponent : public UActorComponent
{
	GENERATED_BODY()

//public:	
//	// Sets default values for this component's properties
//	UMyMonsterStatComponent();
//
//protected:
//	virtual void BeginPlay() override;
//
//public:
//	void SetHP(int NewHP);
//	void SetMaxHp(int MaxHp);
//	void ApplyDamage(float DamageAmount);
//
//	UPROPERTY(Transient, VisibleInstanceOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
//	float CurrentHP;
//
//	int TransManaCost;
//	int NewMaxHP;
//	int32 currentSpeed;
//	FString MonsterName;
//
//	UEntityWidget* EntityWidget;
//	void UpdateWidget();
//
};
