// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

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

    //if (MonsterBehaviorTree != nullptr)
    //{
    //    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("behaviortree found")));
    //    RunBehaviorTree(MonsterBehaviorTree);

    //    APawn* playerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

    //    if (playerPawn != nullptr)
    //    {
    //        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Get Player Pawn")));
    //        //GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), playerPawn->GetActorLocation());
    //    }
    //    else
    //    {
    //        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Failed")));

    //    }
    //}
    //else
    //{
    //    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("behaviortree not found")));

    //}
}

// Called every frame
void AMyAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}