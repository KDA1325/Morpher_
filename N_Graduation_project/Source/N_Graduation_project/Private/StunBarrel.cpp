
#include "StunBarrel.h"
#include "FireBarrel.h"
#include "Barrel.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "EntityPreset.h"
#include "N_Graduation_project/N_Graduation_projectCharacter.h"

// Sets default values
AStunBarrel::AStunBarrel()
{
	PrimaryActorTick.bCanEverTick = true;

	ExplosionDelay = 1.0f;
	ApplyDamage = false;
	SphereComponent = nullptr;
	DamageAmount = 0.0f;
}

// Called when the game starts or when spawned
void AStunBarrel::BeginPlay()
{
	Super::BeginPlay();
	SphereComponent = Cast<USphereComponent>(GetDefaultSubobjectByName(TEXT("Sphere")));

	if(SphereComponent)
	{
		SphereComponent->SetGenerateOverlapEvents(true);
		//UE_LOG(LogTemp,Log,TEXT("Barrel: SphereComponent 세팅 완료"));
	} else
	{
	//	UE_LOG(LogTemp,Error,TEXT("Barrel: SphereComponent가 NULL입니다! BP에서 설정되었는지 확인하세요"));
	}
}

// Called every frame
void AStunBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// 외부에서 데미지 유입 시 호출
void AStunBarrel::WorkBarrel(float DA)
{
	//GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Red,TEXT("Barrel: 응답받음"));
	DamageAmount = 20;

	StartExplosion();
}

// 폭발 후 실제 데미지 처리 및 연쇄 폭발 처리
void AStunBarrel::EndtExplosion()
{
	if(!SphereComponent)
	{
		UE_LOG(LogTemp,Error,TEXT("Barrel: SphereComponent가 없습니다"));
		return;
	}

	TArray<AActor*> OverlappingActors;
	SphereComponent->GetOverlappingActors(OverlappingActors);

	for(AActor* Actor : OverlappingActors)
	{
		if(!Actor || Actor == this)
			continue;

		for(const FName& Tag : Actor->Tags)
		{
			UE_LOG(LogTemp,Warning,TEXT("폭발- 태그: %s"),*Tag.ToString());
		}

		// Barrel인 경우 연쇄 폭발 처리
		if(Actor->ActorHasTag(FName("Barrel")))
		{
			if(AStunBarrel* StunBarrel = Cast<AStunBarrel>(Actor))
			{
				StunBarrel->WorkBarrel(DamageAmount); // AStunBarrel 고유 로직
			} else if(ABarrel* NormalBarrel = Cast<ABarrel>(Actor))
			{
				NormalBarrel->WorkBarrel(DamageAmount); // ABarrel 로직
			} else if(AFireBarrel* FireBarrel = Cast<AFireBarrel>(Actor)){
				FireBarrel->WorkBarrel(DamageAmount); // FireBarrel 로직

			}
		}
		//기절 처리
		if(Actor->ActorHasTag(FName("Monster")))
		{

			AEntityPreset* Entity = Cast<AEntityPreset>(Actor);
			if(Entity)
			{

				APawn* Pawn = Cast<APawn>(Entity);
				if(Pawn)
				{
					AAIController* AICon = Cast<AAIController>(Pawn->GetController());
					if(AICon)
					{
						Entity->bIsVisibleEffectStun = true;
						UE_LOG(LogTemp,Warning,TEXT("폭발 Monster"));
						AICon->StopMovement();
						UE_LOG(LogTemp,Warning,TEXT("폭몬 StopMovement 호출됨: %s"),*Pawn->GetName());
						if(AICon->BrainComponent)
						{
							AICon->BrainComponent->StopLogic(TEXT("Stunned"));
							UE_LOG(LogTemp,Warning,TEXT("폭몬 StopLogic 호출됨: %s"),*Pawn->GetName());
						}
						else
						{
							UE_LOG(LogTemp,Error,TEXT("폭몬 BrainComponent 없음: %s"),*Pawn->GetName());
						}
						FTimerHandle StunTimer;
						Pawn->GetWorldTimerManager().SetTimer(StunTimer,[AICon]()
						{
							if(AICon && AICon->BrainComponent)
							{
								AEntityPreset* Entity = Cast<AEntityPreset>(AICon);
								
								AICon->BrainComponent->RestartLogic();
							}
						}, 
							2.0f,false
						);
						
					}
				}
				FTimerHandle StunEndHandle;
				FTimerDelegate StunEndDelegate = FTimerDelegate::CreateLambda([=]()
				{
					if(Entity)
					{
						Entity->bIsVisibleEffectStun= false;
						UE_LOG(LogTemp,Warning,TEXT("Fire effect ended on %s"),*Actor->GetName());
					}
				});
				GetWorld()->GetTimerManager().SetTimer(StunEndHandle,StunEndDelegate,1.f,false);
			}
			
		}
		if(Actor->ActorHasTag(FName("Player"))){
			AN_Graduation_projectCharacter* MyChar = Cast<AN_Graduation_projectCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
			MyChar->StartAction();
			MyChar->ApplyStun(2);
			UE_LOG(LogTemp,Warning,TEXT("폭발 Player"));
			
			FTimerHandle TimerHandle;

			// 타이머 세팅 (람다 함수로 직접 C++ 함수 호출)
			GetWorld()->GetTimerManager().SetTimer(TimerHandle,[MyChar]()
			{
				if(MyChar)
				{
					MyChar->EndAction();
				}
			},2.0f,false);
		}
		// 데미지 처리
		AController* InstigatorController = GetInstigatorController();
		UGameplayStatics::ApplyDamage(Actor,DamageAmount,InstigatorController,this,nullptr);
	}
}
