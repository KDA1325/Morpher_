
#include "FrozeFloor.h"
#include "EntityPreset.h"
#include "N_Graduation_project/N_Graduation_projectCharacter.h"
#include "Particles/ParticleSystemComponent.h" 

// Sets default values
AFrozeFloor::AFrozeFloor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFrozeFloor::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Box")));
	if(BoxComponent)
	{
		On_Froze();
		BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore); // 전부 무시하고
		BoxComponent->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap); // 원하는 채널만 오버랩
		BoxComponent->SetCollisionResponseToChannel(ECC_WorldDynamic,ECR_Overlap); // 필요시 추가

		BoxComponent->OnComponentBeginOverlap.AddDynamic(this,&AFrozeFloor::OnOverlapBegin);
		// 델리게이트 바인딩
		UE_LOG(LogTemp,Error,TEXT("Find BoxComponent"));

	} else
	{
		UE_LOG(LogTemp,Error,TEXT("BoxComponent가 NULL"));
	}

	FreezeParticle = Cast<UParticleSystemComponent>(GetDefaultSubobjectByName(TEXT("P_Freeze")));

	if(FreezeParticle)
	{
		UE_LOG(LogTemp,Warning,TEXT("FreezeParticle 연결됨"));
	} else
	{
		UE_LOG(LogTemp,Error,TEXT("FreezeParticle 못 찾음"));
	}

}

// Called every frame
void AFrozeFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFrozeFloor::Off_Froze()
{
	ActiveFroze=false;

	if(FreezeParticle){
		FreezeParticle->DeactivateSystem(); // 파티클 끄기
	}
}

void AFrozeFloor::On_Froze()
{
	ActiveFroze=true;
	if(FreezeParticle){
		FreezeParticle->ActivateSystem(); // 파티클 켜기
	}
	UE_LOG(LogTemp,Warning,TEXT("On_Froze, ActiveFroze %d"),ActiveFroze);
}

void AFrozeFloor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,
	UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult & SweepResult)
{
	if(ActiveFroze==true){
		if(!OtherActor->ActorHasTag("HitBox")){
			if(!ActiveFroze || !OtherActor || OtherActor == this)
				return;
			if(OtherComp->ComponentHasTag("HitBox"))
			{
				UE_LOG(LogTemp,Warning,TEXT("On_Froze 무시됨: %s"),*OtherActor->GetName());
				return;
			}

			if(SlowedActors.Contains(OtherActor))
			{
				// 이미 슬로우 중이면 아무것도 안 함
				return;
			}
			SlowedActors.Add(OtherActor);

			if(Duration > 0.f && SlowFactor > 0.f)
			{
				AEntityPreset* Entity = Cast<AEntityPreset>(OtherActor);
				AN_Graduation_projectCharacter* PlayerCharacter = Cast<AN_Graduation_projectCharacter>(OtherActor);
				if(Entity)
				{
					APawn* Pawn = Cast<APawn>(Entity);
					if(Pawn)
					{
						UCharacterMovementComponent* MoveComp = Pawn->FindComponentByClass<UCharacterMovementComponent>();
						if(MoveComp)
						{
							const float OriginalSpeed = MoveComp->MaxWalkSpeed;
							const float NewSpeed = OriginalSpeed / SlowFactor;

							MoveComp->MaxWalkSpeed = NewSpeed;

							// 복구 타이머 설정
							FTimerHandle RestoreHandle;
							FTimerDelegate RestoreDelegate;

							// Pawn과 OriginalSpeed 캡처
							RestoreDelegate.BindLambda([this,OtherActor,Pawn,OriginalSpeed]() {
								if(Pawn && Pawn->FindComponentByClass<UCharacterMovementComponent>())
								{
									Pawn->FindComponentByClass<UCharacterMovementComponent>()->MaxWalkSpeed = OriginalSpeed;
									SlowedActors.Remove(OtherActor);
									UE_LOG(LogTemp,Warning,TEXT("슬로우 해제: %s → 원래 속도 %.1f 복원됨"),*Pawn->GetName(),OriginalSpeed);
								}

							});

							// Duration 후 타이머 호출
							GetWorld()->GetTimerManager().SetTimer(RestoreHandle,RestoreDelegate,Duration,false);

							UE_LOG(LogTemp,Warning,TEXT("슬로우 적용: %s → 속도 %.1f → %.1f (%.1f초간)"),*Pawn->GetName(),OriginalSpeed,NewSpeed,Duration);
						}
					}
				} else if(PlayerCharacter)
				{
					if(Duration > 0.f && SlowFactor > 0.f)
					{
						UCharacterMovementComponent* PMoveComp = PlayerCharacter->GetCharacterMovement();
						if(PMoveComp)
						{
							float OriginalSpeed = PMoveComp->MaxWalkSpeed;
							float NewSpeed = OriginalSpeed / SlowFactor;

							PMoveComp->MaxWalkSpeed = NewSpeed;

							UE_LOG(LogTemp,Warning,TEXT("Freezing applied to %s → NewSpeed: %.1f for %.1f seconds"),*PlayerCharacter->GetName(),NewSpeed,Duration);

							// 일정 시간 뒤에 원래 속도로 복원
							FTimerHandle RestoreHandle;
							FTimerDelegate RestoreDelegate;

							// 캡처값 반드시 복사
							RestoreDelegate.BindLambda([this,OtherActor,PlayerCharacter,OriginalSpeed]() {
								if(PlayerCharacter && PlayerCharacter->GetCharacterMovement())
								{
									PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = OriginalSpeed;
									SlowedActors.Remove(OtherActor);
									UE_LOG(LogTemp,Warning,TEXT("Freezing ended: Restored speed %.1f to %s"),OriginalSpeed,*PlayerCharacter->GetName());
								}
							});

							GetWorld()->GetTimerManager().SetTimer(RestoreHandle,RestoreDelegate,Duration,false);
						}

					}
				}


				UE_LOG(LogTemp,Warning,TEXT("On_Froze 범위 내 감지된 액터: %s"),*OtherActor->GetName());
			}
		}
	}
}