// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "EnemyFSM.h"
#include "TPSPlayer.h"
#include <Components/CapsuleComponent.h>
#include <Components/BoxComponent.h>

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = false;

	auto Collision = Cast<UCapsuleComponent>(RootComponent);
	if (Collision != nullptr)
	{
		Collision->SetCollisionProfileName(TEXT("Enemy"));
	}


	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonKhaimera/Characters/Heroes/Khaimera/Meshes/Khaimera.Khaimera'"));
	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		GetMesh()->SetRelativeScale3D(FVector(0.85f));
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90.f), FRotator(0, -90.f, 0));
	}

	AttackAreaL = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackAreaL"));
	AttackAreaL->SetBoxExtent(FVector(5.f, 20.f, 15.f));
	AttackAreaL->SetCollisionProfileName(TEXT("EnemyAttackArea"));
	AttackAreaL->SetupAttachment(GetMesh(), TEXT("AttackAreaL"));

	AttackAreaR = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackAreaR"));
	AttackAreaR->SetBoxExtent(FVector(5.f, 20.f, 15.f));
	AttackAreaL->SetCollisionProfileName(TEXT("EnemyAttackArea"));
	AttackAreaR->SetupAttachment(GetMesh(), TEXT("AttackAreaR"));

	FSM = CreateDefaultSubobject<UEnemyFSM>(TEXT("FSM"));

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned; // 스폰 또는 월드 배치시 AIContoller 자동 pose
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	AttackAreaL->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapAttackArea);
	AttackAreaR->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapAttackArea);
	AttackAreaOff();
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::AttackAreaOn()
{
	AttackAreaL->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AttackAreaR->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::AttackAreaOff()
{
	AttackAreaL->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackAreaR->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::OnOverlapAttackArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr && OverlappedComponent->GetCollisionProfileName().Compare(FName(TEXT("Player"))))
	{
		auto Player = Cast<ATPSPlayer>(OtherActor);
		if (Player != nullptr)
		{
			Player->OnAttackDamage(50);
		}
	}
}