// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/LDEnemyFSM.h"
#include "Characters/Enemy.h"
#include "Characters/TPSPlayer.h"
#include <AIController.h>

void ULDEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	AttackRange = LDAttackRange; // ���Ÿ������� ����
}

void ULDEnemyFSM::MoveState() 
{
	if (!Target.IsValid()) return;

	// NavMeshVolume�� ���� ��ǥ���� �̵� AI->MoveToActor(Target.Get()); AI->MoveToLocation(Target->GetActorLocation());
	if (!(AI->GetMoveStatus() == EPathFollowingStatus::Moving))
	{
		FAIMoveRequest Req;
		Req.SetGoalActor(Target.Get());
		AI->MoveTo(Req);
	}
	if (FVector::Distance(Owner->GetActorLocation(), Target->GetActorLocation()) <= AttackRange)
	{
		// TODO : ���� Ʈ���̽��� ���� �÷��̾ ���� ������ ��ġ���� Ȯ��
		bool bCanAttack = IsCanAttackPlayer();
		// Ʈ���̽� ������ ����
		if(bCanAttack) EnemyState = EEnemyState::Attack;
	}
}

void ULDEnemyFSM::AttackState() // TODO : ���Ÿ� �������� ����
{
	AI->StopMovement();
	if (FVector::Distance(Owner->GetActorLocation(), Target->GetActorLocation()) > AttackRange
		|| !IsCanAttackPlayer()) // ������ �� ���� ��ġ�� �ٽ� �̵�
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