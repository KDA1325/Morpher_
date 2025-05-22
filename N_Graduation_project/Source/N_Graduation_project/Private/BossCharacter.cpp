#include "BossCharacter.h"
#include "ManaCoreCrystal.h"
#include "Kismet/GameplayStatics.h"

ABossCharacter::ABossCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	BossPatternManager = CreateDefaultSubobject<ABossPatternManager>(TEXT("BossPatternManager"));

}

void ABossCharacter::BeginPlay()
{
	Super::BeginPlay();
	Pattern1();
}

void ABossCharacter::Pattern1(){
///	if(SkillStart){

	BossPatternManager->Thunder();
	UE_LOG(LogTemp,Warning,TEXT("Thunde Pattern1"));


	//}
}

