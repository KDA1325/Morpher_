	#include "NK_Event.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "MyGameInstance.h"
// Sets default values
ANK_Event::ANK_Event()
{
	PrimaryActorTick.bCanEverTick = true;

	// 생성자에서 컴포넌트를 초기화하지 않음. BeginPlay에서 로드
	BoxComponent = nullptr;

}

// Called when the game starts or when spawned
void ANK_Event::BeginPlay()
{
	Super::BeginPlay();

	// 1) Box 컴포넌트를 이름으로 찾아서 OnBeginOverlap 처리
	BoxComponent = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Box")));
	if(BoxComponent)
	{
		BoxComponent->OnComponentBeginOverlap.AddDynamic(this,&ANK_Event::OnBoxBeginOverlap);
	} 
	WallBPClass = StaticLoadClass(AActor::StaticClass(),nullptr,TEXT("/Game/Object/BP_StoneGate.BP_StoneGate_C"));
	if(WallBPClass)
	{
		UE_LOG(LogTemp,Log,TEXT("WallBPClass 로딩 성공"));
	} else
	{
		UE_LOG(LogTemp,Warning,TEXT("WallBPClass 로딩 실패"));
	}
}

// Called every frame
void ANK_Event::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// OnBeginOverlap 함수에서 몬스터와 충돌 시 벽을 제거하는 기능
void ANK_Event::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,
	UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult)
{
	UE_LOG(LogTemp,Log,TEXT("NK_Event: Overlap 감지"));

	if(OtherActor && OtherActor->ActorHasTag(TEXT("Player")))
	{
		UE_LOG(LogTemp,Log,TEXT("NK_Event: Monster 태그 감지"));

		// BP_NK_Wall 클래스를 찾아서 맵에서 인스턴스를 가져옴
		if(WallBPClass)
		{
			TArray<AActor*> FoundActors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(),WallBPClass,FoundActors);

			if(FoundActors.Num() > 0)
			{
				AActor* WallActor = FoundActors[0];  // 맨 처음에 찾은 WallActor를 가져옴
				ANK_Event* WallEvent = Cast<ANK_Event>(WallActor);

				if(WallActor)
				{

					UMyGameInstance* MyGI = Cast<UMyGameInstance>(GetGameInstance());
					if(MyGI)
					{
						MyGI->GetKey = true;
					}
					if(WallEvent)
					{					
						DestroyEvent();
						WallEvent->DestroyEvent(); // 여기를 추가해야 문이 삭제됨
					}

					UE_LOG(LogTemp,Log,TEXT("NK_Event: WallActor 삭제 완료"));
				}
			} else
			{
				UE_LOG(LogTemp,Warning,TEXT("NK_Event: BP_NK_Wall을 맵에서 찾을 수 없습니다"));
			}
		} else
		{
			UE_LOG(LogTemp,Warning,TEXT("NK_Event: WallBPClass가 nullptr입니다"));
		}
	}
}
