#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_Footstep.generated.h"

/**
 * 발소리 재생용 애님 노티파이
 */
UCLASS()
class N_GRADUATION_PROJECT_API UAnimNotify_Footstep: public UAnimNotify
{
	GENERATED_BODY()

public:
	// 애니메이션 재생 중 노티파이 타이밍에 호출됨
	virtual void Notify(USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Animation) override;
};
