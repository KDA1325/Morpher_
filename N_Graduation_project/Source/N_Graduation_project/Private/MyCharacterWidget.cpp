// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacterWidget.h"
//MyCharacterWidget = CreateDefaultSubobject<UMyCharacterWidget>(TEXT("MyCharacterWidget"));

//void UMyCharacterWidget::GetTime() {
//	CoolTime = PlayerSkillComponent->CoolTimeData;
//	bIsNomalCool = PlayerSkillComponent->CanUseNomalSkill;
//	bIsSpecialCool = PlayerSkillComponent->CanUseSpecialSkill;
//}

void UMyCharacterWidget::CollTimeData(float time, bool IsNomalCool, bool IsSpecialCool) {
	UE_LOG(LogTemp, Warning, TEXT("toto Widget Address: %p, CoolTime Address: %p"), this, &CoolTime);

	if (this == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("toto UMyCharacterWidget is nullptr!"));
		return;
	}
	else {
		UE_LOG(LogTemp, Log, TEXT("toto Widget time: %f bIsNomalCool: %s"), time, IsNomalCool ? TEXT("true") : TEXT("false"));
		CoolTime = time;
		bIsNomalCool = IsNomalCool;
		bIsSpecialCool = IsSpecialCool;
	}
}