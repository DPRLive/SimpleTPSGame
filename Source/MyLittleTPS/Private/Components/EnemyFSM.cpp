// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/EnemyFSM.h"
#include "Characters/Enemy.h"
#include "Characters/TPSPlayer.h"
#include <animation/AnimMontage.h>
#include <AIController.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "ItemDrop.h"

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
	case EEnemyState::LDAttack:
		LDAttackState();
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
	if (FVector::Distance(Owner->GetActorLocation(), Target->GetActorLocation()) <= AttackRange) // 할 수 있으면 근거리 공격
	{
		EnemyState = EEnemyState::Attack;
	}
	else if (FVector::Distance(Owner->GetActorLocation(), Target->GetActorLocation()) <= LDAttackRange && 
		IsCanAttackPlayer()) // 원거리 공격
	{
		EnemyState = EEnemyState::LDAttack;
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
		PlayAnim(FName(*FString::Printf(TEXT("Attack%d"), Rand)), EEnemyState::Move);
	}
}

void UEnemyFSM::LDAttackState()
{
	AI->StopMovement();
	if (FVector::Distance(Owner->GetActorLocation(), Target->GetActorLocation()) > LDAttackRange || !IsCanAttackPlayer())
	{
		EnemyState = EEnemyState::Move;
	}
	else
	{
		PlayAnim("LDAttack", EEnemyState::Move);
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
		Owner->OnEnemyDie(); // 스코어 올리기 위한 블루프린트 이벤트 호출
		// 20프로의 확률로 아이템 드랍
		uint8 Rand = FMath::RandRange(1, 10);
		if (Rand <= 2) GetWorld()->SpawnActor(AItemDrop::StaticClass(), &Owner->GetActorTransform());
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
	Owner->AttackAreaOff(); // 공격하다가 다른 몽타주 처리 시를 대비해 AttackArea를 꺼버림
	EnemyState = EEnemyState::AnimPlay;
	Owner->StopAnimMontage(EnemyMontage);
	Owner->PlayAnimMontage(EnemyMontage, 1.f, AnimName);
	DestState = NewDestState;
}

void UEnemyFSM::OnEndPlayAnim()
{
	EnemyState = DestState;
}

bool UEnemyFSM::IsCanAttackPlayer()
{
	FVector TargetDir = Target->GetActorLocation() - Owner->GetActorLocation();
	TargetDir.Normalize();

	FHitResult Result;
	FVector StartPos = Owner->GetActorLocation();
	FVector EndPos = StartPos + TargetDir * LDAttackRange;
	FCollisionQueryParams Params;
	// TODO : Enemy는 트레이스 X로 변경하기
	
	FCollisionShape SweepSphere = FCollisionShape::MakeSphere(50.f); // 50 짜리 크기 공격 할거임
	// bool bHit = GetWorld()->LineTraceSingleByChannel(Result, StartPos, EndPos, ECollisionChannel::ECC_Visibility, Params);

	//////////////////////////////// trace debug
	const FName TraceTag("MyTraceTag");
	GetWorld()->DebugDrawTraceTag = TraceTag;
	FCollisionQueryParams CollisionParams;
	CollisionParams.TraceTag = TraceTag;
	// /////////////////////////////

	// 스킬 크기만큼 Trace.
	bool bHit = GetWorld()->SweepSingleByProfile(Result, StartPos, EndPos, FQuat(ForceInit), "EnemySkill", SweepSphere, CollisionParams);
	if (bHit)
	{
		auto Player = Cast<ATPSPlayer>(Result.GetActor());

		if (Player != nullptr) return true;
	}
	return false;
}