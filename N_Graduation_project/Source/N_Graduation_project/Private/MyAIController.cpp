// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MyAI.h"
#include "EntityPreset.h"

// Sets default values
AMyAIController::AMyAIController()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    static ConstructorHelpers::FObjectFinder<UBlackboardData> BBMonsterRef(TEXT("/Script/AIModule.BlackboardData'/Game/Entity/BB_Monster.BB_Monster'"));
    if (nullptr != BBMonsterRef.Object)
    {
        BBMonster = BBMonsterRef.Object;
    }

    static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTMonsterRef(TEXT("/Script/AIModule.BehaviorTree'/Game/Entity/BT_Monster.BT_Monster'"));
    if (nullptr != BTMonsterRef.Object)
    {
        BTMonster = BTMonsterRef.Object;
    }
    
}

void AMyAIController::RunAI()
{ 
    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("Run AI")));
    if (BlackboardComp)
    {
        Blackboard->SetValueAsVector(BBKEY_HOMEPOS, GetPawn()->GetActorLocation());

        bool RunResult = RunBehaviorTree(BTMonster);
        ensure(RunResult);
    }
}

void AMyAIController::StopAI()
{
    UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(BrainComponent);

    if (BTComponent)
    {
        BTComponent->StopTree();
    }
}

// Called when the game starts or when spawned
void AMyAIController::BeginPlay()
{
    Super::BeginPlay();
}

void AMyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (InPawn)
    {
        AEntityPreset* PossessedCharacter = Cast<AEntityPreset>(InPawn);
       
        // 캐스팅 실패 시 널 체크
        if (PossessedCharacter)
        {
            // Blackboard 초기화 후 키 값 설정
            if (UseBlackboard(BBMonster, BlackboardComp) && BlackboardComp)
            {
                // 키 값을 Blackboard에 저장
                BlackboardComp->SetValueAsInt(BBKEY_ATTACKTYPE, (uint8)PossessedCharacter->GetAttackType());
                BlackboardComp->SetValueAsFloat(BBKEY_NORMALSKILLRANGE, PossessedCharacter->GetNormalSkillRange());
                BlackboardComp->SetValueAsFloat(BBKEY_SPECIALSKILLRANGE, PossessedCharacter->GetSpecialSkillRange());

                // 스킬 사용 조건 플로우에 사용할 A 스킬, B 스킬 정의
                DefineSkillPriority(PossessedCharacter);

            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("UseBlackboard failed in OnPossess"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Casting to AEntityPreset failed in OnPossess"));
        }
        UE_LOG(LogTemp, Warning, TEXT("Possessed Pawn: %s"), *InPawn->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("OnPossess received a null Pawn"));
    }

    RunAI();
}

// Called every frame
void AMyAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// 스킬 사용 조건 플로우에 사용할 A 스킬, B 스킬 정의
void AMyAIController::DefineSkillPriority(AEntityPreset* PossessedCharacter)
{
    float NormalSkillRange = BlackboardComp->GetValueAsFloat(BBKEY_NORMALSKILLRANGE);
    float SpecialSkillRange = BlackboardComp->GetValueAsFloat(BBKEY_SPECIALSKILLRANGE);

    //FString ASkillID, BSkillID;
    float A_SkillRange, B_SkillRange;

    // A스킬, B스킬 정의를 위한 Skill Range 비교 
    if (NormalSkillRange <= SpecialSkillRange)
    {
        A_SkillRange = NormalSkillRange;
        B_SkillRange = SpecialSkillRange;

        //ASkillID = PossessedCharacter->NormalSkillData.SkillNameID;
        //BSkillID = PossessedCharacter->SpecialSkillData.SkillNameID;
    }
    else
    {
        A_SkillRange = SpecialSkillRange;
        B_SkillRange = NormalSkillRange;

        //ASkillID = PossessedCharacter->NormalSkillData.SkillNameID;
        //BSkillID = PossessedCharacter->SpecialSkillData.SkillNameID;
    }

    BlackboardComp->SetValueAsFloat(BBKEY_ASKILLRANGE, A_SkillRange);
    BlackboardComp->SetValueAsFloat(BBKEY_BSKILLRANGE, B_SkillRange);
    
}