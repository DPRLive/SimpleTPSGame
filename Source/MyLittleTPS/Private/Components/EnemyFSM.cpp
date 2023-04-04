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
	
	// NavMeshVolume�� ���� ��ǥ���� �̵� AI->MoveToActor(Target.Get()); AI->MoveToLocation(Target->GetActorLocation());
	if (!(AI->GetMoveStatus() == EPathFollowingStatus::Moving))
	{
		FAIMoveRequest Req;
		Req.SetGoalActor(Target.Get());
		AI->MoveTo(Req);
	}
	if (FVector::Distance(Owner->GetActorLocation(), Target->GetActorLocation()) <= AttackRange) // �� �� ������ �ٰŸ� ����
	{
		EnemyState = EEnemyState::Attack;
	}
	else if (FVector::Distance(Owner->GetActorLocation(), Target->GetActorLocation()) <= LDAttackRange && 
		IsCanAttackPlayer()) // ���Ÿ� ����
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
	Owner->AttackAreaOff(); // ���� �ִϸ��̼� �߿� ���°� �ٲ���� �� ������ ���� Area üũ ����
	AI->StopMovement();

	if (Hp - Damage <= 0)
	{
		Hp = 0;
		Owner->SetActorEnableCollision(false);
		Owner->OnEnemyDie(); // ���ھ� �ø��� ���� �������Ʈ �̺�Ʈ ȣ��
		// 20������ Ȯ���� ������ ���
		uint8 Rand = FMath::RandRange(1, 10);
		if (Rand <= 2) GetWorld()->SpawnActor(AItemDrop::StaticClass(), &Owner->GetActorTransform());
		PlayAnim(TEXT("Die"), EEnemyState::Die);
	}
	else
	{
		Hp -= Damage;
		// ������ �ִϸ��̼� ����
		uint8 Rand = FMath::RandRange(0, 2);
		PlayAnim(FName(*FString::Printf(TEXT("Hit%d"), Rand)), EEnemyState::Move);
	}
}

void UEnemyFSM::DieState(float DeltaTime)
{ 
	// ���� �ִϸ��̼��� ������ �Ʒ��� �������鼭 ����
	FVector Dist = Owner->GetActorLocation() + (FVector::DownVector * (100 * DeltaTime)); // ��ӵ� �
	Owner->SetActorLocation(Dist);

	DieTime += DeltaTime;
	if (DieTime >= 2.f) Owner->Destroy();
}

void UEnemyFSM::PlayAnim(const FName& AnimName, EEnemyState NewDestState)
{
	Owner->AttackAreaOff(); // �����ϴٰ� �ٸ� ��Ÿ�� ó�� �ø� ����� AttackArea�� ������
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
	// TODO : Enemy�� Ʈ���̽� X�� �����ϱ�
	
	FCollisionShape SweepSphere = FCollisionShape::MakeSphere(50.f); // 50 ¥�� ũ�� ���� �Ұ���
	// bool bHit = GetWorld()->LineTraceSingleByChannel(Result, StartPos, EndPos, ECollisionChannel::ECC_Visibility, Params);

	//////////////////////////////// trace debug
	const FName TraceTag("MyTraceTag");
	GetWorld()->DebugDrawTraceTag = TraceTag;
	FCollisionQueryParams CollisionParams;
	CollisionParams.TraceTag = TraceTag;
	// /////////////////////////////

	// ��ų ũ�⸸ŭ Trace.
	bool bHit = GetWorld()->SweepSingleByProfile(Result, StartPos, EndPos, FQuat(ForceInit), "EnemySkill", SweepSphere, CollisionParams);
	if (bHit)
	{
		auto Player = Cast<ATPSPlayer>(Result.GetActor());

		if (Player != nullptr) return true;
	}
	return false;
}