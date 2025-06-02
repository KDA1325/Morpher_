#include "AnimNotify_Footstep.h"
#include "GameFramework/Character.h"
#include "N_Graduation_project/N_Graduation_projectCharacter.h"

void UAnimNotify_Footstep::Notify(USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Animation)
{
	if(ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner()))
	{
		if(AN_Graduation_projectCharacter* MyChar = Cast<AN_Graduation_projectCharacter>(Character))
		{
			MyChar->PlayFootstepSound();
		}
	}
}
