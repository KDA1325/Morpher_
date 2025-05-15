// Fill out your copyright notice in the Description page of Project Settings.


#include "NK_Ball.h"

// Sets default values
ANK_Ball::ANK_Ball()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ANK_Ball::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANK_Ball::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ANK_Ball::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
 
