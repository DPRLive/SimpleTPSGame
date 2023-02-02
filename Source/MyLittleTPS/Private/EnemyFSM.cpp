// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include "Enemy.h"
#include "TPSPlayer.h"
#include <animation/AnimMontage.h>
#include <AIController.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>
#include <GameFramework/CharacterMovementComponent.h>

UEnemyFSM::UEnemyFSM()
{
	PrimaryComponentTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<UAnimMontage> MontageTemp(TEXT("/Script/Engine.AnimMontage'/Game/Animations/AM_EnemyFullMontage.AM_EnemyFullMontage'"));
	if (MontageTemp.Succeeded()) EnemyMontage = MontageTemp.Object;
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

	switch (EnemyState)
	{
	case EEnemyState::Move:
		MoveState();
		break;
	case EEnemyState::Attack:
		AttackState();
		break;
	case EEnemyState::Die:
		DieState(DeltaTime);
		break;
	case EEnemyState::AnimPlay:
		break;
	}
}

void UEnemyFSM::MoveState()
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
		EnemyState = EEnemyState::Attack;
	}
}

void UEnemyFSM::AttackState()
{
	AI->StopMovement();
	if (FVector::Distance(Owner->GetActorLocation(), Target->GetActorLocation()) > AttackRange)
	{
		EnemyState = EEnemyState::Move;
	}
	else
	{
		uint8 Rand = FMath::RandRange(0, 2);
		PlayAnim(FName(*FString::Printf(TEXT("Attack%d"), Rand)), EEnemyState::Attack);
	}
}

void UEnemyFSM::OnAttackDamage(float Damage)
{
	Owner->AttackAreaOff(); // 공격 애니메이션 중에 상태가 바뀌었을 수 있으니 공격 Area 체크 중지
	AI->StopMovement();

	if (Hp - Damage <= 0)
	{
		Hp = 0;
		Owner->SetActorEnableCollision(false);
		PlayAnim(TEXT("Die"), EEnemyState::Die);
	}
	else
	{
		Hp -= Damage;
		// 데미지 애니메이션 실행
		uint8 Rand = FMath::RandRange(0, 2);
		PlayAnim(FName(*FString::Printf(TEXT("Hit%d"), Rand)), EEnemyState::Move);
	}
}

void UEnemyFSM::DieState(float DeltaTime)
{ 
	// 죽음 애니메이션이 끝나면 아래로 내려가면서 퇴장
	FVector Dist = Owner->GetActorLocation() + (FVector::DownVector * (100 * DeltaTime)); // 등가속도 운동
	Owner->SetActorLocation(Dist);

	DieTime += DeltaTime;
	if (DieTime >= 2.f) Owner->Destroy();
}

void UEnemyFSM::PlayAnim(const FName& AnimName, EEnemyState NewDestState)
{
	EnemyState = EEnemyState::AnimPlay;
	Owner->StopAnimMontage(EnemyMontage);
	Owner->PlayAnimMontage(EnemyMontage, 1.f, AnimName);
	DestState = NewDestState;
}

void UEnemyFSM::OnEndPlayAnim()
{
	EnemyState = DestState;
}
