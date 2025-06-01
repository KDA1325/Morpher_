#include "AnimNotify_ShowHitBox.h"
#include "EntityPreset.h"
#include "ABGameSingleton.h"
#include "TimerManager.h"
#include "N_Graduation_project/N_Graduation_projectCharacter.h"
void UAnimNotify_ShowHitBox::Notify(USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Animation)
{
	if(!MeshComp)
		return;

	AEntityPreset* Entity = Cast<AEntityPreset>(MeshComp->GetOwner());
	if(!Entity)
		return;

	Entity->ShowHitBox();  // 내부에서 bIsCharging 판단 후 알아서 분기
	//UE_LOG(LogTemp,Error,TEXT("AnimNotify_ShowHitBox called — HitBox Shown"));

	UE_LOG(LogTemp,Error,TEXT("[NOTIFY] ShowHitBox called — %s"),*Entity->GetName());

	if(Entity->bIsBreaking)
	{
		UE_LOG(LogTemp,Error,TEXT("[NOTIFY] bIsBreaking is TRUE"));
	} else if(Entity->bIsCharging)
	{
		UE_LOG(LogTemp,Error,TEXT("[NOTIFY] bIsCharging is TRUE"));
	} else if(Entity->bIsBreath)
	{
		UE_LOG(LogTemp,Error,TEXT("[NOTIFY] bIsBreath is TRUE"));
	} else
	{
		UE_LOG(LogTemp,Error,TEXT("[NOTIFY] Normal Skill will show"));
	}
}


//void UAnimNotify_ShowHitBox::Notify(USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Animation)
//{
//	if(!MeshComp || SkillID.IsEmpty())
//		return;
//
//	AEntityPreset* Entity = Cast<AEntityPreset>(MeshComp->GetOwner());
//	if(!Entity)
//		return;
//
//	FSkillData SkillData;
//	if(UABGameSingleton::Get().GetSkillDataBySkillID(SkillID,SkillData))
//	{
//		// 히트박스 활성화
//		if(SkillID == "Skill_Charge")
//		{
//			Entity->ShowSpecialHitBox();
//		} else
//		{
//			Entity->ShowNormalHitBox();
//		}
//
//		UE_LOG(LogTemp,Error,TEXT("AnimNotify_ShowHitBox() called"));
//
//		//// Duration 후 히트박스 비활성화
//		//FTimerHandle TimerHandle;
//		//float Duration = SkillData.SkillDuration;
//
//		//MeshComp->GetWorld()->GetTimerManager().SetTimer(TimerHandle,[Entity,SkillID = this->SkillID]()
//		//{
//		//	if(SkillID == "Skill_Charge")
//		//	{
//		//		Entity->HideSpecialHitBox();
//		//	} else
//		//	{
//		//		Entity->HideNormalHitBox();
//		//	}
//		//},Duration,false);
//	} else
//	{
//		UE_LOG(LogTemp,Warning,TEXT("AnimNotify_ShowHitBox: SkillID %s not found in SkillDataMap"),*SkillID);
//	}
//}
