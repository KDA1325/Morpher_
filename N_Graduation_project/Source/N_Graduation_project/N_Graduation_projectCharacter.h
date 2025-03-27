#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "ABEntityData.h" // Entity Data 구조체
#include "ABGameSingleton.h"

#include "N_Graduation_projectCharacter.generated.h"

class UPlayerSkillComponent;
class UMyPlayerStatComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UTimelineComponent;
struct FInputActionValue;
class UCharacterStateComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config = Game, Blueprintable)

class AN_Graduation_projectCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Dash Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DashAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MouseLeftClickAction;

	void NomalSkillAction(const FInputActionValue& Value);

public:
	AN_Graduation_projectCharacter();


protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for Dash input */
	// 대시를 시전하면 도착하는 위치로 이동할 수 있는지 확인
	void DashCheck(const FInputActionValue& Value);

	// 대시 기능 수행 
	void Dash(const FVector DashDir, const FVector DashVel);
	void EndDash();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime);

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

private:
	// Dash 거리
	UPROPERTY(EditAnywhere, Category = "Dash", meta = (AllowPrivateAccess = "true"))
	float DashDistance;

	// Dash를 수행할 타임라인
	UPROPERTY()
	UTimelineComponent* DashTimeline;

	// 타임라인에 사용할 커브
	UPROPERTY()
	UCurveFloat* DashCurve;

	// 타임라인에 있는 커브가 수행되면서 실행될 함수 
	UFUNCTION()
	void DashInterpReturn(float value);

	// Dash를 수행하는 방향
	FVector DashDirection;
	// Dash를 수행할 때의 속력 
	FVector DashVelocity;

	void OnPlayerDead();
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPlayerSkillComponent* PlayerSkillComponent;

	UPROPERTY(VisibleAnywhere)
	UCharacterStateComponent* CharacterStateComponent;
	/** 체력 컴포넌트 */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UMyPlayerStatComponent* PlayerStatComponent;


public:
	
	/** 데미지 받는 함수 */
	UFUNCTION(BlueprintCallable, Category = "Player Stats")
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	//액터가 받은 대미지를 처리하는 로직을 추가하기 위해 오버라이드.
	//DamageAmount 데미지의 양
	//FDamageEvent const& DamageEvent 데미지 종류
	//EventInstigator,//데미지를 준 컨트롤러
	//DamageCauser//데미지를 준 액터 자체

	// 무적 상태 활성화

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsInvincible;

	UFUNCTION(BlueprintNativeEvent)
	void On_invincibility();


	// 실제 위젯 인스턴스
	/*private:
		TSubclassOf<UUserWidget> CharacterHealthBarWidgetClass;
		UUserWidget* CharacterHealthBarWidget;
	void SpawnWidget();*/

	// 캐릭터 변신 메소드
	/*UFUNCTION(EditAnywhere, BlueprintCallable, Category = "Stat")
	void TransformToEntity(int32 EntityID);*/

	// 설정한 GroupID를 키 값으로 가져온 데이터를 적용하는 함수
	UFUNCTION(BlueprintCallable, Category = "Stat")
	void UpdateEntityData();
	//// 변신 후 현재 체력 업데이트
	//UFUNCTION(EditAnywhere, BlueprintCallable, Category = "Stat")
	//void UpdateHealth(float NewHealth);

	// 현재 캐릭터 데이터 저장
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	FABEntityData EntityData;

	/* 캐릭터의 현재 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float CurrentHealth;*/

	// 메시 변경 테스트를 위해 
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* m_pMeshCom;

	int32 maxHp;
	int32 moveSpeed;
	FString normalSkill;
	FString specialSkill;
	FString presetReference;

	int32 currentSpeed;
	int32 OriginalSpeed;
	FString currentPreset;
	FString pastPreset;
	
	
	// MoveSpeed를 설정하는 함수
	void SetMoveSpeed(int32 MoveSpeed);
	void StartAction();
	void EndAction();
	// Preset을 설정하는 함수
	void SetPreset(FString PresetReference);

	//데미지 테스트
//	void DealDamageToPlayer();

	bool bIsMoving;

};

