// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/ArrowComponent.h"// Sets default values
#include "Totem_Projectile.h"
#include "Totem.generated.h"

UCLASS()
class N_GRADUATION_PROJECT_API ATotem : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATotem();

	UFUNCTION(BlueprintCallable)
		void TotemProjectile();
	UPROPERTY(EditAnywhere)
		USoundBase* BreakObjectSound;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
	UPROPERTY(BlueprintReadWrite)
		int TotemHP;

	UPROPERTY(BlueprintReadWrite)
		int ProjectileSpeed;

	UPROPERTY(BlueprintReadWrite)
		float AttackDelay;

	UPROPERTY(BlueprintReadWrite)
		float ApplyDamageAmount;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Attack")
		TSubclassOf<ATotem_Projectile> ProjectileClass;

	// 발사 위치 기준이 될 얼굴 컴포넌트
	UPROPERTY(VisibleAnywhere,Category = "Components")
		UArrowComponent* FaceComponent;
	UPROPERTY(VisibleAnywhere)
		class USceneComponent* Root;
	// 투사체 스폰 타이머
	FTimerHandle AttackTimerHandle;

	float TakeDamage(float DamageAmount,FDamageEvent const& DamageEvent,AController* EventInstigator,AActor* DamageCauser);

};
