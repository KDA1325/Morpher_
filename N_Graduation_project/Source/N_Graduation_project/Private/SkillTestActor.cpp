#include "SkillTestActor.h"
#include "PlayerSkillComponent.h"

// Sets default values
ASkillTestActor::ASkillTestActor()
{
	// 이 액터는 매 프레임 갱신되지 않음
	PrimaryActorTick.bCanEverTick = true;

	// PlayerSkillComponent를 생성하여 액터에 추가
	PlayerSkillComponent = CreateDefaultSubobject<UPlayerSkillComponent>(TEXT("PlayerSkillComponent"));
}

// Called when the game starts or when spawned
void ASkillTestActor::BeginPlay()
{
	Super::BeginPlay();

	// 방어 스킬을 테스트로 활성화
	PlayerSkillComponent->OnDefenseSkill();
}

// Called every frame
void ASkillTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
