// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/LDEnemyFSM.h"
#include "Characters/Enemy.h"
#include "Characters/TPSPlayer.h"
#include <AIController.h>

void ULDEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	AttackRange = LDAttackRange; // 원거리용으로 변경
}

void ULDEnemyFSM::MoveState() 
{
	if (!Target.IsValid()) return;

	// NavMeshVolume을 통해 목표에게 이동 AI->MoveToActor(Target.Get()); AI->MoveToLocation(Target->GetActorLocation());
	if (!(AI->GetMoveStatus() == EPathFollowingStatus::Moving))
	{
		FAIMoveRequest Req;
		Req.SetGoalActor(Target.Get());
		AI->MoveTo(Req);
	}
	if (FVector::Distance(Owner->GetActorLocation(), Target->GetActorLocation()) <= AttackRange)
	{
		// TODO : 라인 트레이스를 통해 플레이어를 공격 가능한 위치인지 확인
		bool bCanAttack = IsCanAttackPlayer();
		// 트레이스 성공시 공격
		if(bCanAttack) EnemyState = EEnemyState::Attack;
	}
}

void ULDEnemyFSM::AttackState() // TODO : 원거리 공격으로 변경
{
	AI->StopMovement();
	if (FVector::Distance(Owner->GetActorLocation(), Target->GetActorLocation()) > AttackRange
		|| !IsCanAttackPlayer()) // 공격할 수 없는 위치면 다시 이동
	{
		EnemyState = EEnemyState::Move;
	}
	else
	{
		uint8 Rand = FMath::RandRange(0, 2);
		PlayAnim(FName(*FString::Printf(TEXT("Attack%d"), Rand)), EEnemyState::Attack);
	}
}

bool ULDEnemyFSM::IsCanAttackPlayer()
{
	FHitResult Result;
	FVector StartPos = Owner->GetActorLocation();
	FVector EndPos = StartPos + Owner->GetActorForwardVector() * LDAttackRange;
	FCollisionQueryParams Params;
	bool bHit = GetWorld()->LineTraceSingleByProfile(Result, StartPos, EndPos, FName("Player"), Params);
	return bHit;
}