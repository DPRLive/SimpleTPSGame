// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/EnemyFSM.h"
#include "Characters/Enemy.h"
#include "Characters/TPSPlayer.h"
#include "ItemDrop.h"

#include <animation/AnimMontage.h>
#include <AIController.h>
#include <Kismet/GameplayStatics.h>
#include <GameFramework/CharacterMovementComponent.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(EnemyFSM)

UEnemyFSM::UEnemyFSM()
{
	PrimaryComponentTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<UAnimMontage> MontageTemp(TEXT("/Script/Engine.AnimMontage'/Game/Animations/AM_EnemyFullMontage.AM_EnemyFullMontage'"));
	if (MontageTemp.Succeeded()) EnemyMontage = MontageTemp.Object;
}

void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();
	
	if (Owner = Cast<AEnemy>(GetOwner()))
	{
		AI = Cast<AAIController>(Owner->GetController());
		Cast<UCharacterMovementComponent>(Owner->GetMovementComponent())->MaxWalkSpeed = WalkSpeed;
		Owner->OnEnemyDie.AddDynamic(this, &UEnemyFSM::EnemyDie);
	}
	
	if (auto Actor = UGameplayStatics::GetActorOfClass(GetWorld(), ATPSPlayer::StaticClass()))
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
	TraceInterval += GetWorld()->GetDeltaSeconds();
	
	// NavMeshVolume을 통해 목표에게 이동
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
	else if (FVector::Distance(Owner->GetActorLocation(), Target->GetActorLocation()) <= LDAttackRange) // 원거리 공격
	{
		// 이동하며 1초에 한번씩 트레이스 진행
		bool bCanAttackPlayer = false;
		if(TraceInterval > 1.f)
		{
			bCanAttackPlayer = IsCanAttackPlayer();
			TraceInterval = 0.f;
		}
		if(bCanAttackPlayer) EnemyState = EEnemyState::LDAttack;
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
	if (FVector::Distance(Owner->GetActorLocation(), Target->GetActorLocation()) <= AttackRange ||
		FVector::Distance(Owner->GetActorLocation(), Target->GetActorLocation()) > LDAttackRange || !IsCanAttackPlayer())
	{
		EnemyState = EEnemyState::Move;
	}
	else
	{
		PlayAnim("LDAttack", EEnemyState::LDAttack);
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

void UEnemyFSM::EnemyDie()
{
	AI->StopMovement();
	// 20프로의 확률로 아이템 드랍
	uint8 Rand = FMath::RandRange(1, 10);
	if (Rand <= 2) GetWorld()->SpawnActor(AItemDrop::StaticClass(), &Owner->GetActorTransform());
	PlayAnim(TEXT("Die"), EEnemyState::Die);
}

void UEnemyFSM::PlayAnim(const FName& AnimName, EEnemyState NewDestState)
{
	Owner->AttackAreaOff(); // 공격하다가 다른 몽타주 처리 시를 대비해 AttackArea를 꺼버림
	EnemyState = EEnemyState::AnimPlay;
	Owner->StopAnimMontage(EnemyMontage);
	Owner->PlayAnimMontage(EnemyMontage, 1.f, AnimName);
	DestState = NewDestState;
}

bool UEnemyFSM::IsCanAttackPlayer()
{
	FVector TargetDir = Target->GetActorLocation() - Owner->GetActorLocation();
	TargetDir.Normalize();

	FHitResult Result;
	FVector StartPos = Owner->GetActorLocation();
	FVector EndPos = StartPos + TargetDir * LDAttackRange;
	FCollisionQueryParams Params;
	
	FCollisionShape SweepSphere = FCollisionShape::MakeSphere(80.f); // 공격 크기 100 이하 라고 합시다

	// 스킬 크기만큼 Trace.
	bool bHit = GetWorld()->SweepSingleByProfile(Result, StartPos, EndPos, FQuat(ForceInit), "EnemySkill", SweepSphere);
	if (bHit)
	{
		auto Player = Cast<ATPSPlayer>(Result.GetActor());

		if (Player != nullptr) return true;
	}
	return false;
}

void UEnemyFSM::TakeDamage()
{
	AI->StopMovement();
	// 데미지 애니메이션 실행
	uint8 Rand = FMath::RandRange(0, 2);
	PlayAnim(FName(*FString::Printf(TEXT("Hit%d"), Rand)), EEnemyState::Move);
}

void UEnemyFSM::OnEndPlayAnim()
{
	EnemyState = DestState;
}
