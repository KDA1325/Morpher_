#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ABGameSingleton.h"
#include "SkillData.h"
#include "SkillEffectData.h"
#include "Components/BoxComponent.h"  // UBoxComponent
#include "Components/ArrowComponent.h"  // UArrowComponent
#include "ActionAnimInstance.h"

#include "PlayerSkillComponent.generated.h"

//class Forward declarations;
class AActor;
class UBoxComponent;
class UArrowComponent;


DECLARE_MULTICAST_DELEGATE(FOnAction);


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class N_GRADUATION_PROJECT_API UPlayerSkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:


	// Sets default values for this component's properties
	UPlayerSkillComponent();

	FOnAction OnAction;
	float CoolTimeData;
	// 스킬 관련 변수들
	bool CanUseNomalSkill;  // 일반 스킬 사용 가능 여부
	bool CanUseSpecialSkill; // 스페셜 스킬 사용 가능 여부
	bool IsDefending;        // 방어 중인지 여부
	bool OnceHitBox;
	// 현재 스킬 ID
	FString CurrentSkillID;

	// 히트박스 관련 변수들
	UPROPERTY()
	UBoxComponent* PlayerHitBox;

	void SetHitBox(UBoxComponent* NewHitBox);

	// 거리 감지
	UFUNCTION(BlueprintCallable, Category = "Skill")
	float MeasureDistanceToMonster() const;
	//몬스터감지
	AActor* FindFrontMonsterTarget() const;

	//UFUNCTION(BlueprintNativeEvent) // 시간있음 재정의로 해보기
	void VisibleShapeBox(const FString& SkillID);    // 히트박스 보이게
	void NomalSkillPlay(const FString& SkillID);    // 스킬
	void SpecialSkillPlay(const FString& SkillID);    // 스킬
	// 함수들
	void OnDefenseSkill(float Count);          // 방어 스킬 활성화 함수
	void OffDefenseSkill();                    // 방어 스킬 비활성화 함수
	void NomalCooldown();          // 스킬 쿨타임 초기화
	void SpecialCooldown();          // 스킬 쿨타임 초기화
	float GetDistanceTo(const AActor* OtherActor) const;    // 거리 계산 함수
	// 히트박스 초기화 및 활성화 함수
	void SettingHitBox(const FSkillData& SkillData); // 히트박스 초기화
	void OnHitBox(const FSkillData& SkillData);
	// 히트박스 활성화
	void HideHitBox();     // 히트박스 비활성화
	void SkillAnimation(const FString& EffectID);
	void EndSkillAnimation(UAnimMontage* Montage, bool bInterrupted);
	void SkillEffect(const FString& SkillNameID, AActor* TargetActor);
	//타이머
	void SetSkillTimer(float Count, FTimerDelegate Call);  // 타이머 설정 함수
	void SpecialSetSkillTimer(float Count, FTimerDelegate Call);  // 타이머 설정 함수
	void ChargeSkillTimer(float Delay, FTimerDelegate Call);  // 타이머 설정 함수


	UPROPERTY(BlueprintReadOnly)
	float DamageAmount;

	UPROPERTY()
	TSet<AActor*> DamagedActors; // 데미지를 받은 몬스터 저장

protected:
	virtual void BeginPlay() override;

private:
	// 타이머 핸들
	FTimerHandle NomalSkillTimerHandle;
	FTimerHandle SpecialSkillTimerHandle;
	FTimerHandle ChargeSkillTimerHandle;

	// 플레이어와의 거리
	float distance;
public:
	//돼지
	// 돌진 스킬 실행 시 저장할 방향 (설정 후 변화 없이 유지)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	FVector StoredDashDirection;

	void ExecuteChargeDash(FVector Chargedistance);
	void DrawChargePath();
	void DelayedKnockbackEffect();
	void ApplyKnockback(AActor* TargetActor, float KnockbackPower);
};

