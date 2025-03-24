#include "ActionAnimInstance.h"


UActionAnimInstance::UActionAnimInstance()
{
	/*static ConstructorHelpers::FObjectFinder<UAnimMontage> M_SlashMontage(TEXT("/Game/Animation/Sample/retarget_Stable_Sword_Outward_Slash_Anim_mixamo_com"));
	if (M_SlashMontage.Succeeded())
	{
		M_Slash = M_SlashMontage.Object;
	}*/

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

	if (EffectID=="Skill_Slash")
	{
		UE_LOG(LogTemp, Warning, TEXT("Playing Animation: %s"), *M_Slash->GetName());
		Montage_Play(M_Slash);  
	}
}
//FSoftObjectPath MeshPath(TEXT("/Script/Engine.SkeletalMesh'/Game/Animation/Boar/Boar_idle_test3s_3.Boar_idle_test3s_3'"));
//    FSoftObjectPath AnimPath(TEXT("/Script/Engine.AnimSequence'/Game/Animation/Boar/Boar_idle_test3s_2_Anim.Boar_idle_test3s_2_Anim'"));

//    USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshPath.TryLoad());
//    UAnimSequence* LoadedAnim = Cast<UAnimSequence>(AnimPath.TryLoad());
//D:/GitHub/N-Graduation-project/N_Graduation_project/Content/Animation/Sample/retarget_Stable_Sword_Outward_Slash_Anim_mixamo_com_Montage.uasset