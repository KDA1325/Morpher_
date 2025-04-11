#include "SkillTestActor.h"

// Sets default values
ASkillTestActor::ASkillTestActor()
{
	//매 프레임 갱신xx
	PrimaryActorTick.bCanEverTick = true;
	// 기본값 설정
	IsInvincible = false;

	// PlayerSkillComponent를 생성하여 액터에 추가
	PlayerSkillComponent = CreateDefaultSubobject<UPlayerSkillComponent>(TEXT("PlayerSkillComponent"));
}

// Called when the game starts or when spawned
void ASkillTestActor::BeginPlay()
{
	Super::BeginPlay();

	// 방어 on
	//PlayerSkillComponent->OnDefenseSkill(3.0f);
	//무적 on
	On_invincibility();
}

// Called every frame
void ASkillTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


// 무적
void ASkillTestActor::On_invincibility()
{
	if (PlayerSkillComponent && !IsInvincible)
	{
		// 무적 상태 활성화
		IsInvincible = true;

		// PlayerSkillComponent에서 방어 스킬을 실행
		PlayerSkillComponent->OnDefenseSkill(1.0f);

		// 깜박이기구현..해야함
		
	}
}
