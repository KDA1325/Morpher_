
#include "Barrel.h"
#include "Kismet/GameplayStatics.h" //ApplyDamage에 필요
//#include "Engine/Classes/Components/StaticMeshComponent.h"

// Sets default values
ABarrel::ABarrel()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SphereComponent = nullptr;
	ExplosionDelay=1.0f;
	ApplyDamage = false;
}

// Called when the game starts or when spawned
void ABarrel::BeginPlay()
{
	Super::BeginPlay();
	SphereComponent = Cast<USphereComponent>(GetDefaultSubobjectByName(TEXT("Sphere")));
	RootComponent = SphereComponent;
}

// Called every frame
void ABarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void ABarrel::WorkBarrel(float DA)
{
	GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Red,TEXT("Barrel, 응답받았습니다"));
	DamageAmount=DA;
	//	StartBarrel= true;
	//	ChangeColor(ExplosionDelay);
	StartExplosion();

}


void ABarrel::OnExplosionOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult)
{
	UE_LOG(LogTemp,Warning,TEXT("Barrel3 OtherActor Name: %s"),*OtherActor->GetName());

	if(OtherActor && OtherActor != this)
	{
	//	UE_LOG(LogTemp,Warning,TEXT("Barrel3 OtherActor && OtherActor != this"));

	//	//if(ApplyDamage== true){
	//		// 데미지 주는 로직
	//		AController* InstigatorController = GetInstigatorController();	
	//		UGameplayStatics::ApplyDamage(OtherActor,DamageAmount,InstigatorController,this,nullptr);
	//	//	UE_LOG(LogTemp,Warning,TEXT("Barrel DamageAmount: %f"),DamageAmount);
	//		UE_LOG(LogTemp,Warning,TEXT("Barrel3 ApplyDamage== true"));

	//	//}		
	//} 
	//if(OtherActor->ActorHasTag(FName("Barrel")))
	//{
	//	UE_LOG(LogTemp,Warning,TEXT("Barrel: Barrel이여 작동하거라"));
	//	ABarrel* Barrel = Cast<ABarrel>(OtherActor);
	//	if(Barrel)
	//	{
	//		Barrel->WorkBarrel(DamageAmount);
	//	}
	}
}

