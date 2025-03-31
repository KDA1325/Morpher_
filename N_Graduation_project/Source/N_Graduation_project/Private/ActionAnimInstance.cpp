#include "ActionAnimInstance.h"
#include "CharacterStateComponent.h"
#include <N_Graduation_project/N_Graduation_projectCharacter.h>

UActionAnimInstance::UActionAnimInstance()
{

	FSoftObjectPath MontagePath(TEXT("/Game/Animation/Sample/retarget_Stable_Sword_Outward_Slash_Anim_mixamo_com_Montage"));

	// 애셋 로드
	M_Slash = Cast<UAnimMontage>(MontagePath.TryLoad());
	if (M_Slash)
	{
		UE_LOG(LogTemp, Warning, TEXT("Successfully loaded Montage: %s"), *M_Slash->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load Montage from path: %s"), *MontagePath.ToString());
	}

}
void UActionAnimInstance::PlayAnimation(const FString& EffectID)
{
	if (EffectID == "Skill_Slash")
	{
		UE_LOG(LogTemp, Warning, TEXT("Playing Animation: %s"), *M_Slash->GetName());
		Montage_Play(M_Slash);
		// 애니메이션 종료 이벤트 바인딩
		OnMontageEnded.AddDynamic(this, &UActionAnimInstance::OnMontageEndCallback);
	}
}

void UActionAnimInstance::OnMontageEndCallback(UAnimMontage* Montage, bool bInterrupted)
{
	// 애니메이션 종료 후 상태 변경
	UE_LOG(LogTemp, Error, TEXT("Animation Ended"));

	AActor* OwnerActor = GetOwningActor();
	// 애니메이션 종료 후 바인딩 연결
	AN_Graduation_projectCharacter* MyCharacter = Cast<AN_Graduation_projectCharacter>(OwnerActor);
	if (MyCharacter)
	{
		MyCharacter->EndAction(); // EndAction 호출하여 속도 복구
	}

	// 델리게이트에서 호출된 후, 다음 호출을 막기 위해 델리게이트에서 바인딩 해제
	OnMontageEnded.RemoveDynamic(this, &UActionAnimInstance::OnMontageEndCallback);
}
