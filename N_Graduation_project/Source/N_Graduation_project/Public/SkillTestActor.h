#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerSkillComponent.h" // PlayerSkillComponent 포함
#include "SkillTestActor.generated.h"

UCLASS()
class N_GRADUATION_PROJECT_API ASkillTestActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASkillTestActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// 무적 상태 활성화
	void On_invincibility();

private:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 무적 상태 확인
	bool IsInvincible;

	// PlayerSkillComponent를 액터에 추가
	UPROPERTY(VisibleAnywhere)
	UPlayerSkillComponent* PlayerSkillComponent;

protected:
	virtual float TakeDamage(float DamageAmount, 
		struct FDamageEvent const& DamageEvent, 
		class AController* EventInstigator, 
		AActor* DamageCauser) override;

	void DealDamageToPlayer();
};
