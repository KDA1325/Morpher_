//// Fill out your copyright notice in the Description page of Project Settings.
//
//
//#include "IdleState.h"
//#include "CharacterStateComponent.h"
//
//void UIdleState::Enter(AN_Graduation_projectCharacter* Character)
//{
//	// Idle 상태 진입시 수행할 동작 구현.    
//	Character->PlayIdleAnimation(); //캐릭터의 Idle 애니메이션 재생.
//}
//
//void UIdleState::Update(AN_Graduation_projectCharacter* Character, float DeltaTime)
//{
//	// Idle 상태 업데이트 동작    
//	if (Character->IsMoving()) //캐릭터가 움직이고 있으면,    
//	{
//		Character->ChangeState(Character->GetRunState()); //Run 상태로 전환.   
//	}
//	else if (Character->IsJumping()) //캐릭터가 점프하는 중이면,    
//	{
//		Character->ChangeState(Character->GetJumpState()); //점프 상태로 전환.    
//	}
//	else if (Character->IsAttacking()) //캐릭터가 공격하는 중이면,     
//	{
//		Character->ChangeState(Character->GetAttackState()); //공격 상태로 전환.     
//	}
//}
//
//void UIdleState::Exit(AN_Graduation_projectCharacter* Character)
//{    
//	// Idle 상태 탈출시 수행할 동작
//}
