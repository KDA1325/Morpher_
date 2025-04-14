// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EngineMinimal.h" // USkeletalMeshComponent를 사용하기 위해 변경
#include "ABEntityData.h" // Entity Data 구조체
#include "ABGameSingleton.h"
#include "GameFramework/Character.h"
#include "Components/WidgetComponent.h"
//#include "CharacterAllInterface.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimBlueprint.h"
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


	FABEntityData EntityData;
	FSkillData NormalSkillData;
	FSkillData SpecialSkillData;
	TArray<FSkillEffectData> NormalSkillEffectData;
	TArray<FSkillEffectData> SpecialSkillEffectData;

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

	UPROPERTY(EditAnywhere, Category = "Skill")
	UAnimMontage* NormalSkillMontage;
	UPROPERTY(EditAnywhere, Category = "Skill")
	UAnimMontage* SpecialSkillMontage;

	//스킬의 히트박스 컴포넌트를 생성 및 설정하는 함수 
	UFUNCTION(BlueprintCallable, Category = "Skill")
	void SetupHitBoxComponent(FSkillData& SkillData);

	void ConfigureHitBox(UBoxComponent* HitBox);

	UFUNCTION(BlueprintCallable, Category = "Skill")
	void ShowNormalHitBox();

	UFUNCTION(BlueprintCallable, Category = "Skill")
	void HideNormalHitBox();
	UFUNCTION(BlueprintCallable, Category = "Skill")
	void ShowSpecialHitBox();

	UFUNCTION(BlueprintCallable, Category = "Skill")
	void HideSpecialHitBox();

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

	UFUNCTION(BlueprintCallable, Category = "Skill")
	void PerformSkill_Charge();
	UFUNCTION(BlueprintCallable, Category = "Skill")
	void ExecuteChargeDash();

	void ClearCastingSkill();

	// Skill 몽타주 종료 콜백 
	UFUNCTION()
	void OnSkillMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	//// Special Skill 몽타주 종료 콜백 
	//UFUNCTION()
	//void OnSpecialSkillMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void DrawChargePath();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	FString currentEntityGroupID;

	UPROPERTY(EditAnywhere)
	FString currentPreset;
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
