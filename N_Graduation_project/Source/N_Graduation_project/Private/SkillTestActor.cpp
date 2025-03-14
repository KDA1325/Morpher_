#include "SkillTestActor.h"
#include "Kismet/GameplayStatics.h"//ApplyDamage테스트
#include "GameFramework/Character.h"//ApplyDamage테스트
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
	//DealDamageToPlayer();
	//방어 on
	PlayerSkillComponent->OnDefenseSkill(3.0f);
	//무적 on
	//On_invincibility();
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

float ASkillTestActor::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//CurrentAttacker = Cast<AMyPlayer>(DamageCauser);
	//DamagedEffect->Activate(true); //피격 효과 재생
	//if (MonsterAnim) MonsterAnim->PlayDamagedMontage();//피격 애니메이션 재생. 

	return Super::TakeDamage(10, DamageEvent, EventInstigator, DamageCauser);

}

// ApplyDamage 함수 구현
void ASkillTestActor::DealDamageToPlayer()
{
	UE_LOG(LogTemp, Error, TEXT("50 Damage"));

	//0번 플레이어를 가져온다
	ACharacter* TargetCharacter = Cast<ACharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));	
	float DamageAmount = 50.0f;
	// TargetCharacter에서 GetController를 호출
	AController* InstigatorController = TargetCharacter->GetController();
	AActor* DamageCauser = this; // 데미지를 주는 액터 (이 예에서는 적)
	TSubclassOf<UDamageType> DamageType = UDamageType::StaticClass(); // 기본 데미지 타입

	// 데미지 적용
	UGameplayStatics::ApplyDamage(TargetCharacter, DamageAmount, InstigatorController, DamageCauser, DamageType);

}
