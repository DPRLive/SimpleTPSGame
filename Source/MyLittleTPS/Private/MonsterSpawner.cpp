// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterSpawner.h"
#include "Characters/Enemy.h"

#include <Components/CapsuleComponent.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(MonsterSpawner)

AMonsterSpawner::AMonsterSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	CapsuleComp->SetCollisionProfileName(TEXT("NoCollision"));
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(CapsuleComp);
}

void AMonsterSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMonsterSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMonsterSpawner::SpawnMonster() const
{
	GetWorld()->SpawnActor<AEnemy>(EnemyFactory, GetActorLocation(), FRotator::ZeroRotator);
}

