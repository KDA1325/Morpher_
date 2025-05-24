// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss_Laser.h"

// Sets default values
ABoss_Laser::ABoss_Laser()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABoss_Laser::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABoss_Laser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

