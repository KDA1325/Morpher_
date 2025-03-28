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
    //ConstructorHelpers::FObjectFinder<UBehaviorTree>aiBehavior(TEXT("/Script/AIModule.BehaviorTree'/Game/Entity/BT_Monster.BT_Monster'"));
    //GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("aiBehavior called")));

    //if (aiBehavior.Succeeded())
    //{
    //    MonsterBehaviorTree = aiBehavior.Object;
    //    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("aiBehavior.Succeeded")));
    //}
    //else
    //{
    //    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Failed")));

    //}

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
    BlackboardComp = Blackboard.Get();

    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("Run AI")));
    if (UseBlackboard(BBMonster, BlackboardComp))
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
    //
    //ConstructorHelpers::FObjectFinder<UBehaviorTree>aiBehavior(TEXT("/Script/AIModule.BehaviorTree'/Game/Entity/BT_Monster.BT_Monster'"));
    //GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("aiBehavior called")));

    //if (aiBehavior.Succeeded())
    //{
    //    MonsterBehaviorTree = aiBehavior.Object;
    //    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("aiBehavior.Succeeded")));
    //}
    //else
    //{
    //    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Failed")));

    //}

}

void AMyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (InPawn)
    {
        // Blackboard 초기화 후 AttackType 설정
        if (UseBlackboard(BBMonster, BlackboardComp) && BlackboardComp)
        {
            // 캐스팅 실패 시 널 체크
            AEntityPreset* PossessedCharacter = Cast<AEntityPreset>(InPawn);
            if (PossessedCharacter)
            {
                // EnumAttackType 값을 uint8로 캐스팅하여 Blackboard에 저장
                BlackboardComp->SetValueAsEnum(BBKEY_ATTACKTYPE, (uint8)PossessedCharacter->GetAttackType());
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("UseBlackboard failed in OnPossess"));
        }
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