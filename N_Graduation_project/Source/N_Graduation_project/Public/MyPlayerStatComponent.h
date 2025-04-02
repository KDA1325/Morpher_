#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ABEntityData.h" // Entity Data 구조체
#include "ABGameSingleton.h"
#include <N_Graduation_project/N_Graduation_projectCharacter.h>
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"

#include "MyPlayerStatComponent.generated.h"

//델리게이트 못쓰겠음. 
//DECLARE_MULTICAST_DELEGATE(FOnHpIsZeroDelegate);
//DECLARE_MULTICAST_DELEGATE(FOnHpChangedDelegate);
//DECLARE_MULTICAST_DELEGATE(FOnManaChangedDelegate);
class UMyPlayerStatComponent;

UCLASS(ClassGroup = (Custom), meta = (Blueprintable))
class N_GRADUATION_PROJECT_API UMyPlayerStatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UMyPlayerStatComponent();

protected:
    virtual void BeginPlay() override;
    virtual void InitializeComponent() override;

public:
    void TransformToEntity(FString Name, int HP, int Mana);
    AN_Graduation_projectCharacter* OwnerPlayer;
    FString MonsterName;

    // hp 관련
    void SetHP(float NewHP);
    void ApplyDamage(float DamageAmount);

    float NewMaxHP;
    float PastMaxHP;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    UMyPlayerStatComponent* PlayerStatComponent;

    UPROPERTY(Transient, VisibleInstanceOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
    float CurrentHP;

    // 마나 관련
    void SetMaxHp(int MaxHp);
    void SetMana(int NewMana);
    void UseMana(int ManaAmount);
    void RegenerateMana();

    UPROPERTY(Transient, VisibleInstanceOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
    int CurrentMana;

    FTimerHandle ManaRegenTimerHandle;

    //위젯 관련
    UPROPERTY()
    class UUserWidget* HUDWidget;

    UFUNCTION()
    void UpdateHUD();

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UUserWidget> HUDClass;

    void IconChange();

    TArray<UImage*> ManaTokenImages;
    TArray<UImage*> IconImages;

};
