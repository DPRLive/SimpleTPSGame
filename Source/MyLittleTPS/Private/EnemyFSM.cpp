// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include "Enemy.h"
#include "TPSPlayer.h"
#include <AIController.h>
#include <Kismet/GameplayStatics.h>
#include <GameFramework/CharacterMovementComponent.h>

UEnemyFSM::UEnemyFSM()
{
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<AEnemy>(GetOwner());
	if (Owner != nullptr)
	{
		AI = Cast<AAIController>(Owner->GetController());
		Cast<UCharacterMovementComponent>(Owner->GetMovementComponent())->MaxWalkSpeed = WalkSpeed;
	}

	auto Actor = UGameplayStatics::GetActorOfClass(GetWorld(), ATPSPlayer::StaticClass());	
	if (Actor != nullptr)
	{
		Target = Cast<ATPSPlayer>(Actor);
	}

	EnemyState = EEnemyState::Move;
}

void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CurrentTime += DeltaTime;
	switch (EnemyState)
	{
	case EEnemyState::Move:
		MoveState();
		break;
	case EEnemyState::Attack:
		AttackState();
		break;
	case EEnemyState::Damaged:
		DamageState();
		break;
	case EEnemyState::Die:
		DieState(DeltaTime);
		break;
	}
}

void UEnemyFSM::MoveState()
{
	if (!Target.IsValid()) return;
	
	// NavMeshVolume을 통해 목표에게 이동
	AI->MoveToActor(Target.Get());
	//AI->MoveToLocation(Target->GetActorLocation());

	if (FVector::Distance(Owner->GetActorLocation(), Target->GetActorLocation()) <= AttackRange)
	{
		AI->StopMovement();
		Attack();
		EnemyState = EEnemyState::Attack;
	}
}

void UEnemyFSM::Attack()
{
	UE_LOG(LogTemp, Warning, TEXT("Attack!!"));
	CurrentTime = 0;
}

void UEnemyFSM::AttackState()
{
	if (CurrentTime >= AttackDelay)
	{
		Attack();
	}
	if (FVector::Distance(Owner->GetActorLocation(), Target->GetActorLocation()) > AttackRange)
	{
		EnemyState = EEnemyState::Move;
	}
}

void UEnemyFSM::OnAttackDamage(float Damage)
{
	AI->StopMovement();
	EnemyState = EEnemyState::Damaged;
	// 데미지 애니메이션 실행
	if (Hp - Damage < 0) Hp = 0;
	else Hp -= Damage;
}

void UEnemyFSM::DamageState()
{
	if (Hp > 0) // 데미지 애니메이션 끝났는지 확인 필요
	{
		EnemyState = EEnemyState::Move;
	}
	else
	{
		EnemyState = EEnemyState::Die;
	}
}

void UEnemyFSM::DieState(float DeltaTime)
{
	// 죽음 애니메이션 실행
	// 죽음 애니메이션이 끝나면
	// 아래로 내려가면서 퇴장
	Owner->SetActorEnableCollision(false);
	FVector Dist = Owner->GetActorLocation() + (FVector::DownVector * (100 * DeltaTime)); // 등가속도 운동
	Owner->SetActorLocation(Dist);

	if (Owner->GetActorLocation().Z <= -300) Owner->Destroy();
}
