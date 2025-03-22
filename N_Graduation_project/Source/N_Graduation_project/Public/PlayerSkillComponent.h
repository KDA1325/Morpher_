#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ABGameSingleton.h"
#include "SkillData.h"
#include "Components/BoxComponent.h"  // UBoxComponent
#include "Components/ArrowComponent.h"  // UArrowComponent
//#include "MyAnimInstance.h"

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

	// 스킬 관련 변수들
	bool CanUseNomalSkill;  // 일반 스킬 사용 가능 여부
	bool CanUseSpecialSkill; // 스페셜 스킬 사용 가능 여부
	bool IsDefending;        // 방어 중인지 여부
	bool OnceHitBox; 
	// 히트박스 관련 변수들
	UPROPERTY()
	UBoxComponent* HitBox;
	UPROPERTY()
	UArrowComponent* Arrow;
	
	// 현재 스킬 ID
	FString CurrentSkillID;

	AActor* FindMonsterTarget();

	//UFUNCTION(BlueprintNativeEvent) // 시간있음 재정의로 해보기
	void NomalSkillType(const FString& SkillID);    // 노말스킬 타입 설정 함수
	void SpecialSkillType(const FString& SkillID);    // 스페셜스킬 타입 설정 함수

	// 함수들
	void OnDefenseSkill(float Count);          // 방어 스킬 활성화 함수
	void OffDefenseSkill();                    // 방어 스킬 비활성화 함수
	void NomalCooldown();          // 스킬 쿨타임 초기화
	void SpecialCooldown();          // 스킬 쿨타임 초기화
	void SetSkillTimer(float Count, FTimerDelegate Call);  // 타이머 설정 함수
	float GetDistanceTo(const AActor* OtherActor) const;    // 거리 계산 함수
	//void ActivateShieldEffect(const FString& EffectID); //이펙트값 넣을 함수
	// 히트박스 초기화 및 활성화 함수
	void SettingHitBox(const FSkillData& SkillData); // 히트박스 초기화
	void OnHitBox(const FSkillData& SkillData);                            // 히트박스 활성화
	void HideHitBox();     // 히트박스 비활성화
	//void SkillAnimation(const FString& EffectID);

	// 애니메이션 
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
	//UMyAnimInstance* MyAnimInstance;
	//AActor* FindMonsterTarget();

protected:
	virtual void BeginPlay() override; 

private:
	// 타이머 핸들
	FTimerHandle TimerHandle;

	// 플레이어와의 거리
	float distance;

};

