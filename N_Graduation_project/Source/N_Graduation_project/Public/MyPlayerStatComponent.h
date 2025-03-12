#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ABGameSingleton.h" // 싱글톤 클래스 포함
#include "MyPlayerStatComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class N_GRADUATION_PROJECT_API UMyPlayerStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMyPlayerStatComponent();

protected:
	virtual void BeginPlay() override;

public:
	// 체력과 관련된 변수들
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float MaxHealth;

	UPROPERTY(BlueprintReadOnly, Category = "Player Stats")
	float CurrentHealth;

	// 체력 감소 함수
	UFUNCTION(BlueprintCallable, Category = "Player Stats")
	void TakeDamage(float DamageAmount);

	// 체력 회복 함수
	//UFUNCTION(BlueprintCallable, Category = "Player Stats")
	//void Heal(float HealAmount);

	// 데미지 받을 때 이벤트 발생
	//UFUNCTION(BlueprintImplementableEvent, Category = "Player Stats")
	//void OnHealthChanged(float Health);

	// 죽었을 때 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category = "Player Stats")
	void OnDeath();

};