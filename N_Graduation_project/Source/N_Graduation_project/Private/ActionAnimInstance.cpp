#include "ActionAnimInstance.h"
#include "CharacterStateComponent.h"
#include <N_Graduation_project/N_Graduation_projectCharacter.h>

UActionAnimInstance::UActionAnimInstance()
{

	FSoftObjectPath SlashMontagePath(TEXT("/Game/Gamin/Player/Player_Attack/Player_attack_Anim_Montage.Player_attack_Anim_Montage"));

	// 애셋 로드
	M_Slash = Cast<UAnimMontage>(SlashMontagePath.TryLoad());
	//if (M_Slash)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Successfully loaded Montage: %s"), *M_Slash->GetName());
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Error, TEXT("Failed to load Montage from path: %s"), *SlashMontagePath.ToString());
	//}
	
	//FSoftObjectPath BiteMontagePath(TEXT("/Game/Animation/WildBoar_Bite_Anim_Montage.WildBoar_Bite_Anim_Montage")); //기존 몽타주
	FSoftObjectPath BiteMontagePath(TEXT("/Game/Gamin/Bore_UVW/Bore_attack_uvw_Anim_Montage.Bore_attack_uvw_Anim_Montage"));

	// 애셋 로드
	M_Bite = Cast<UAnimMontage>(BiteMontagePath.TryLoad());
	if (M_Bite)
	{
		UE_LOG(LogTemp, Warning, TEXT("Successfully loaded Montage: %s"), *M_Bite->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load Montage from path: %s"), *BiteMontagePath.ToString());
	}

}
void UActionAnimInstance::PlayAnimation(const FString& EffectID)
{
	UE_LOG(LogTemp, Warning, TEXT("Playing Animation 함수 실행됨, EffectID: %s"), *EffectID);
	if (EffectID == "Skill_Slash")
	{
		UE_LOG(LogTemp, Warning, TEXT("Playing Animation: %s"), *M_Slash->GetName());
		Montage_Play(M_Slash);
		// 애니메이션 종료 이벤트 바인딩
		OnMontageEnded.AddDynamic(this, &UActionAnimInstance::OnMontageEndCallback);
	}
	//if (EffectID == "Skill_FireBall") {
	//	UE_LOG(LogTemp, Warning, TEXT("Playing Animation: %s"), *M_Slash->GetName());
	//	Montage_Play(M_Slash);
	//	// 애니메이션 종료 이벤트 바인딩
	//	OnMontageEnded.AddDynamic(this, &UActionAnimInstance::OnMontageEndCallback);
	//}
	if (EffectID == "Skill_Bite" || EffectID=="Skill_Charge") {
		UE_LOG(LogTemp, Warning, TEXT("Playing Animation: %s"), *M_Bite->GetName());
		UE_LOG(LogTemp, Warning, TEXT("Playing Skill_Bite 실행됨"));
		Montage_Play(M_Bite);
	//	float PlayResult = Montage_Play(M_Bite);
	//	UE_LOG(LogTemp, Warning, TEXT("Montage_Play result: %f"), PlayResult);

		// 애니메이션 종료 이벤트 바인딩
		OnMontageEnded.AddDynamic(this, &UActionAnimInstance::OnMontageEndCallback);
	}

	else{
		UE_LOG(LogTemp, Warning, TEXT("Playing Animation 해당하는 스킬 없음"));

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
		MyCharacter->EndAction(); // EndAction 호출
		UE_LOG(LogTemp, Warning, TEXT("OnMontageEndCallback EndAction실행됨 "));
	}

	// 델리게이트에서 바인딩 해제
	OnMontageEnded.RemoveDynamic(this, &UActionAnimInstance::OnMontageEndCallback);
}
