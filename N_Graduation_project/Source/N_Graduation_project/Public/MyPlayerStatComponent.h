#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ABEntityData.h" // Entity Data 구조체
#include "ABGameSingleton.h"
#include <N_Graduation_project/N_Graduation_projectCharacter.h>

#include "MyPlayerStatComponent.generated.h"

//델리게이트 못쓰겠음. 
//DECLARE_MULTICAST_DELEGATE(FOnHpIsZeroDelegate);
//DECLARE_MULTICAST_DELEGATE(FOnHpChangedDelegate);
//DECLARE_MULTICAST_DELEGATE(FOnManaChangedDelegate);


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class N_GRADUATION_PROJECT_API UMyPlayerStatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UMyPlayerStatComponent();

protected:
    virtual void BeginPlay() override;
    virtual void InitializeComponent() override;

public:
    void SetHP(int NewHP);
    void SetMaxHp(int MaxHp);

    void SetMana(int NewMana);
    void UseMana(int ManaAmount);
    void ApplyDamage(float DamageAmount);

    void TransformToEntity(int HP, int Mana);
    bool CanTransform(int TransManaCost) const;

   // ECharacterState GetCurrentState() const { return CurrentState; }
   // void SetCurrentState(ECharacterState NewState);
   
    //UPROPERTY(BlueprintAssignable)
   // FOnHpIsZeroDelegate OnHPIsZero;

    int TransManaCost;
    int NewMaxHP;
    float PastMaxHP;
    float PastHP;

    AN_Graduation_projectCharacter* OwnerPlayer;
  
    UPROPERTY(Transient, VisibleInstanceOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
    float CurrentHP;

    UPROPERTY(Transient, VisibleInstanceOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
    int CurrentMana;

  

    //UPROPERTY(BlueprintAssignable)
    //FOnHpChangedDelegate OnHPChanged;

    //UPROPERTY(BlueprintAssignable)
    //FOnManaChangedDelegate OnManaChangedDelegate;
    
};
