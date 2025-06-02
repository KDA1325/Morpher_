// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EngineMinimal.h" // USkeletalMeshComponent를 사용하기 위해 변경
#include "ABEntityData.h" // Entity Data 구조체
#include "ABGameSingleton.h"
#include "Components/TimelineComponent.h"
#include "Components/DecalComponent.h" // 데칼 인디케이터용
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimBlueprint.h"
#include "NiagaraComponent.h"
#include "EntityPreset.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float, NewHealth);
class UEntityWidget;
class UEntitySkillComponent;

UCLASS()
class N_GRADUATION_PROJECT_API AEntityPreset : public ACharacter
{
	GENERATED_BODY()

public:
	// 델리게이트 선언
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnHealthChanged OnHealthChanged;

	// Sets default values for this character's properties
	AEntityPreset();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget")
	UEntityWidget* EntityWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UWidgetComponent* WidgetComp; //액터에 붙이는 컴포넌트(블루프린트 위젯으로 지정)

	UEntitySkillComponent* EntitySkillComponent;
	// 블루프린트에서 할당하는 WidgetComponent (이 컴포넌트에 위젯 블루프린트가 지정되어 있어야 함)
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	//UWidgetComponent* WidgetComp;

	UPROPERTY(EditAnywhere)
	float CurrentHP;

	UPROPERTY(EditAnywhere)
	USoundBase* DeathSound;
	
	UPROPERTY(EditAnywhere)
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere)
	USoundBase* GuardHitSound;

	UPROPERTY(EditAnywhere)
	USoundBase* ShieldEndSound;

	UPROPERTY(EditAnywhere)
	USoundBase* FireSound;

	UPROPERTY(EditAnywhere)
		USoundBase* FireEffectHitSound;

	UPROPERTY(EditAnywhere)
		USoundBase* ChargeEffectHitSound;

	UPROPERTY(EditAnywhere)
		USoundBase* ArmSwingEffectHitSound;

	UPROPERTY(EditAnywhere)
	FString SoundSkillID;

	FABEntityData EntityData;
	FSkillData NormalSkillData;
	FSkillData SpecialSkillData;
	TArray<FSkillEffectData> NormalSkillEffectData;
	TArray<FSkillEffectData> SpecialSkillEffectData;

	UPROPERTY(EditAnywhere)
	FString currentPreset;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FString NormalSkillID;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FString SpecialSkillID;


	// HitBox를 위한 컨테이너 컴포넌트 (소켓 기준)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	USceneComponent* NormalHitBoxContainer;	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	USceneComponent* SpecialHitBoxContainer;

	// Normal 스킬용 히트박스 컴포넌트 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	UBoxComponent* NormalSkillHitBox;
	// Special 스킬용 히트박스 컴포넌트 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	UBoxComponent* SpecialSkillHitBox;
	// Special 스킬용 Sphere 히트박스 컴포넌트 
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Skill")
	USphereComponent* SpecialSkillSphereHitBox;

	UPROPERTY(EditAnywhere, Category = "Skill")
	UAnimMontage* NormalSkillMontage;
	UPROPERTY(EditAnywhere, Category = "Skill")
	UAnimMontage* SpecialSkillMontage;


	//UPROPERTY(EditDefaultsOnly,Category = "Skill")
	//TSubclassOf<class AEntityProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Projectile")
	TSubclassOf<class AEntityProjectile> NormalProjectileClass;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Projectile")
	TSubclassOf<class AEntityProjectile> SpecialProjectileClass;

	//// 손에 붙일 화살
	//UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Projectile")
	//AEntityProjectile* PendingArrow;  

	// 노말 화살 or 스페셜 센터 화살 
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Projectile")
	AEntityProjectile* Arrow;
	
	// 스페셜 서브 화살 
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Projectile")
	AEntityProjectile* SubArrow1;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Projectile")
	AEntityProjectile* SubArrow2;

	//UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Projectile")
	//UChildActorComponent* SkillArrowChildComponent;



	//스킬의 히트박스 컴포넌트를 생성 및 설정하는 함수 
	UFUNCTION(BlueprintCallable, Category = "Skill")
	void SetupHitBoxComponent(FSkillData& SkillData);

	void ConfigureHitBox(UBoxComponent* HitBox);

	void ConfigureSphereHitBox(USphereComponent * HitBox);

	UFUNCTION(BlueprintCallable, Category = "Skill")
	void ShowNormalHitBox();

	UFUNCTION(BlueprintCallable, Category = "Skill")
	void HideNormalHitBox();
	UFUNCTION(BlueprintCallable, Category = "Skill")
	void ShowSpecialHitBox();
	UFUNCTION(BlueprintCallable, Category = "Skill")
	void ShowSpecialSphereHitBox();

	UFUNCTION(BlueprintCallable, Category = "Skill")
	void HideSpecialHitBox();
	UFUNCTION(BlueprintCallable, Category = "Skill")
	void HideSpecialSphereHitBox();

	void ShowHitBox();

	//void AnimNotify_ShowHitBox();
	//void AnimNotify_SpawnProjectile();

	//void AnimNotify_SpawnProjectile_FireBall();

	// 히트박스 Overlap 이벤트 처리 함수
	UFUNCTION()
	void OnNormalHitBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);
	UFUNCTION()
	void OnSpecialHitBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	// 스킬 시전 중 플래그: 돌진 스킬 시전 중엔 이동 업데이트 차단
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	bool bIsCastingSkill;
	// 돌진 스킬 실행 시 저장할 방향 (설정 후 변화 없이 유지)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	FVector StoredDashDirection;

	// 방어 중 플래그
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	bool bIsDefending;

	UFUNCTION(BlueprintCallable, Category = "Skill")
	void PerformSkill_Charge();
	void PerformSkill_FireBall();

	//UFUNCTION(BlueprintCallable,Category = "Sound")
	//void PlayFireSound();

	void PerformSkill_FreezeBreath();
	void PerformSkill_EarthBreaker();

	void PerformSkill_Arrow();

	void PerformSkill_SplinterArrow();

	//void SpawnProjectile_Arrow();
	void FireProjectile_Arrow();

	// Skill_Arrow 시전 시 저장할 방향 (설정 후 변화 없이 유지)
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Skill")
	FVector ArrowDirection;

	UFUNCTION(BlueprintCallable, Category = "Skill")
	void ExecuteChargeDash();

	UFUNCTION(BlueprintCallable,Category = "Skill")
	void SpawnProjectile_ThrowRock();

	void SpawnProjectile_FireBall();

	void ClearCastingSkill();

	// Skill 몽타주 종료 콜백 
	UFUNCTION()
	void OnSkillMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	//// Special Skill 몽타주 종료 콜백 
	//UFUNCTION()
	//void OnSpecialSkillMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// Skill_Charge 타임라인
	UPROPERTY()
	UTimelineComponent* ChargeTimeline; // WildBoar_ChargeFloat

	UPROPERTY(EditDefaultsOnly, Category = "Skill|Charge")
	UCurveFloat* ChargeCurve;

	void StartChargeMovement();

	// 타임라인 델리게이트 함수 
	UFUNCTION()
	void Timeline_ChargeProgress(float Value);

	//UFUNCTION()
	//void Timeline_ChargeFinished();

	UFUNCTION()
	void SpawnChargeIndicator(FVector ChargeStartLocation, FVector ChargeTargetLocation);

	// 데칼 표시용
	UPROPERTY()
	UDecalComponent* ChargeDecalComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Skill|Charge")
	UMaterialInstance* ChargeDecalMaterial;

	// 돌진 관련 상태 변수
	bool bIsCharging = false;
	// 프리징 관련 상태 변수
	bool bIsBreath = false;
	// 스턴 관련 상태 변수 
	bool bIsBreaking = false;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Effect")
	bool bIsVisibleEffectFire = false;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Effect")
	bool bIsVisibleEffectFreezing = false;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Effect")
	bool bIsVisibleEffectStun = false;

	//UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Effects")
	//UNiagaraComponent* FireEffectComp;

	//UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Effects")
	//UNiagaraComponent* FreezingEffectComp;

	//UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Effects")
	//UNiagaraComponent* StunEffectComp;


	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Effects",meta=(AllowPrivateAccess = "true"))
		UNiagaraComponent* FreezingEffectComp;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Effects",meta=(AllowPrivateAccess = "true"))
		UNiagaraComponent* StunEffectComp;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Effects",meta=(AllowPrivateAccess = "true"))
		UNiagaraComponent* FireEffectComp;


	UPROPERTY()
	float WildBoar_ChargeFloat;

	FVector ChargeStartLocation;
	FVector ChargeDirection;
	float ChargeDistance = 0.0f;
	FVector ChargeTargetLocation;

	void OnGuardEnded();

	void StopMovement();

	void DrawChargePath();

	UFUNCTION(BlueprintCallable, Category = "Skill")
	void ApplyKnockbackEffect(ACharacter* Target, float Distance, float Duration);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// 최대 체력와 이동 속도
	float MaxHp;
	int32 currentSpeed;

	UPROPERTY(EditAnywhere)
	EnumAttackType currentAttackType;
	
	UPROPERTY(EditAnywhere)
	float currentNormalSkillRange;

	UPROPERTY(EditAnywhere)
	float currentSpecialSkillRange;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Entity Data로 초기화 
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void InitializeEntity(FABEntityData& InEntityData);

	// MaxHP를 설정하는 함수
	UFUNCTION(BlueprintCallable, Category = "Data")
	void SetHP(float NewHP);
	void DelayedDestroy();

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Materials")
	UMaterialInterface* DeathMaterial;

	// MoveSpeed를 설정하는 함수
	UFUNCTION(BlueprintCallable, Category = "Data")
	void SetMoveSpeed(int32 MoveSpeed);

	UFUNCTION(BlueprintCallable, Category = "Data")
	void SetNormalSkillRange(float NormalSkillRange);

	UFUNCTION(BlueprintCallable, Category = "Data")
	void SetSpecialSkillRange(float SpecialSkillRange);

	void ApplyDamage(float DamageAmount);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	float GetHPRatio(); // 최대 HP로 나누기

	//void Spawn_CenterArrow();

	void Fire_AllArrows();

	void PerformSkill_ShieldGuard();

	UFUNCTION(BlueprintCallable, Category = "Data")
	EnumAttackType GetAttackType();
	
	UFUNCTION(BlueprintCallable, Category = "Data")
	float GetNormalSkillRange();
	
	UFUNCTION(BlueprintCallable, Category = "Data")
	float GetSpecialSkillRange();

	//UFUNCTION(BlueprintCallable, Category = "Skill")
	//void PerformNormalSkill();

	//UFUNCTION(BlueprintCallable, Category = "Skill")
	//void PerformSpecialSkill();

// AI Section
protected:
	//virtual float GetAIPatrolRadius() override;
	//virtual float GetAIDetectRange() override;
	//virtual float GetAIAttackRange() override;
	//virtual float GetAITurnSpeed() override;

};
