#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "ABEntityData.h" // Entity Data ����ü
#include "ABGameSingleton.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
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
class UWidgetActor;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthChanged);

UCLASS(config = Game, Blueprintable)

class AN_Graduation_projectCharacter : public ACharacter
{
	GENERATED_BODY()

	//UFUNCTION()
	//void OnHealthChanged();
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
	UInputAction* LeftClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RightClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RightReleasedClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PieMenuAction;

public:
	AN_Graduation_projectCharacter();
	void ChangePreset(FString Name);
//	void LoadPreset(FString PresetID);
	void ToggleMaterial(bool bUseHitMaterial);
	bool PlaySpecial = false;
	bool PlayNomal = false;
	bool OkTrans = true;
	bool bCanMove = true;
	bool noDamage = false;
protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	//void Look(const FInputActionValue& Value);
	void OnPieMenuPressed();//�� ��������
	void OnPieMenuReleased(); //�� ��

	void RotateCharacterToCursor();

	/** Called for Dash input */
	// ��ø� �����ϸ� �����ϴ� ��ġ�� �̵��� �� �ִ��� Ȯ��
	void DashCheck(const FInputActionValue& Value);

	// ��� ��� ���� 
	void Dash(const FVector DashDir, const FVector DashVel);

	//���콺 ��Ŭ��
	void NomalSkillAction(const FInputActionValue& Value);
	void SpecialSkillAction(const FInputActionValue& Value);
	void EndShield();

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

	UPROPERTY()
	UWidgetActor* WidgetActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	UStaticMeshComponent* PlayerSword;

private:
	FVector MouseWorldPosition;
	FVector MouseWorldDirection;

	// Dash �Ÿ�
	UPROPERTY(EditAnywhere, Category = "Dash", meta = (AllowPrivateAccess = "true"))
	float DashDistance;

	// Dash�� ������ Ÿ�Ӷ���
	UPROPERTY()
	UTimelineComponent* DashTimeline;

	// Ÿ�Ӷ��ο� ����� Ŀ��
	UPROPERTY()
	UCurveFloat* DashCurve;

	// Ÿ�Ӷ��ο� �ִ� Ŀ�갡 ����Ǹ鼭 ����� �Լ� 
	UFUNCTION()
	void DashInterpReturn(float value);

	// Dash�� �����ϴ� ����
	FVector DashDirection;
	// Dash�� ������ ���� �ӷ� 
	FVector DashVelocity;





public:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPlayerSkillComponent* PlayerSkillComponent;
		
	UPROPERTY(VisibleAnywhere)
	UCharacterStateComponent* CharacterStateComponent;

	/** ü�� ������Ʈ */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UMyPlayerStatComponent* PlayerStatComponent;
	/** ������ �޴� �Լ� */
	UFUNCTION(BlueprintCallable, Category = "Player Stats")
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


	// ���� ���� Ȱ��ȭ
	UPROPERTY(BlueprintReadWrite)
	bool TestMode = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IsInvincible;

	UFUNCTION(BlueprintNativeEvent)
	void On_invincibility();

	UFUNCTION(BlueprintCallable)
	void OnPlayerDead();
	// ���� ���� �ν��Ͻ�
	/*private:
		TSubclassOf<UUserWidget> CharacterHealthBarWidgetClass;
		UUserWidget* CharacterHealthBarWidget;
	void SpawnWidget();
*/
//UFUNCTION()
//void UpdateHUD();

//UPROPERTY(EditDefaultsOnly, Category = "UI")
//TSubclassOf<class UUserWidget> HUDClass;

//UPROPERTY()
//class UUserWidget* HUDWidget;


// ĳ���� ���� �޼ҵ�
/*UFUNCTION(EditAnywhere, BlueprintCallable, Category = "Stat")
void TransformToEntity(int32 EntityID);*/

// ������ GroupID�� Ű ������ ������ �����͸� �����ϴ� �Լ�
	UFUNCTION(BlueprintCallable, Category = "Stat")
	void UpdateEntityData();


	// ���� ĳ���� ������ ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	FABEntityData EntityData;

	/* ĳ������ ���� ü��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float CurrentHealth;*/

	// �޽� ���� �׽�Ʈ�� ���� 
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* m_pMeshCom;

	UPROPERTY(EditAnywhere)
	float currentHP;

	int changeCount=0;
	int32 maxHp;
	int32 moveSpeed;
	FString NomalSkill="Skill_Slash";
	FString SpecialSkill;
	FString presetReference;

	int32 currentSpeed;
	int32 OriginalSpeed;
	FString currentPreset= "PCPreset.uasset";
	FString pastPreset= "PCPreset.uasset";


	// MoveSpeed�� �����ϴ� �Լ�
	void SetMoveSpeed(int32 MoveSpeed);
	void StartAction();
	void EndAction();
	// Preset�� �����ϴ� �Լ�
	void SetPreset(FString PresetReference);

	//������ �׽�Ʈ
//	void DealDamageToPlayer();

	bool bIsMoving;
	bool bcanPie;
	// �������� ��Ʈ�ڽ��� ���Ͽ� �����ϴ� �Լ� ����
	UFUNCTION(BlueprintCallable, Category = "HitBox")
	void SpawnHitBoxAtSocket(FName SocketName);
	UFUNCTION(BlueprintCallable, Category = "HitBox")
	void SpawnHitBoxAtSocket2(FName SocketName);

	void TrySpawnHitBox(FName SocketName);
	UFUNCTION(BlueprintCallable,Category = "HitBox")
	void SpawnHitSphereAtSocket(FName SocketName);
	UFUNCTION()
	void OnHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY()
		UMaterialInterface* HitMaterial = nullptr;

	UPROPERTY()
	UMaterialInterface* InvincibleOriginalMaterial = nullptr;
	int Tcount;
	//감박이 타이머
	FTimerHandle FlashTimerHandle1;
	FTimerHandle FlashTimerHandle2;
	FTimerHandle FlashTimerHandle3;
	FTimerHandle FlashTimerHandle4;

	bool isDead=false;

};

/*

	pastPreset = "PlayerCharacter";
	NomalSkill = "Skill_Slash";

	currentPreset = " ";
	
	*/