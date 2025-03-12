#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "N_Graduation_projectCharacter.generated.h"

class UPlayerSkillComponent;
class UMyPlayerStatComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UTimelineComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config = Game)
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

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay();
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

	UPROPERTY(VisibleAnywhere)
	UPlayerSkillComponent* PlayerSkillComponent;


public:
	/** 체력 컴포넌트 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Stats")
	class UMyPlayerStatComponent* PlayerStatComponent;

	/** 데미지 받는 함수 */
	UFUNCTION(BlueprintCallable, Category = "Player Stats")
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	//액터가 받은 대미지를 처리하는 로직을 추가하기 위해 오버라이드.
	//DamageAmount 데미지의 양
	//FDamageEvent const& DamageEvent 데미지 종류
	//EventInstigator,//데미지를 준 컨트롤러
	//DamageCauser//데미지를 준 액터 자체
	
	// 무적 상태 활성화
	void On_invincibility();
	bool IsInvincible;


	// 실제 위젯 인스턴스
	private:
		TSubclassOf<UUserWidget> CharacterHealthBarWidgetClass;
		UUserWidget* CharacterHealthBarWidget;
	void SpawnWidget();
};

