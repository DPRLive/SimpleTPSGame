// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySkill.h"
#include <Components/SphereComponent.h>
#include <Particles/ParticleSystem.h>
#include <Particles/ParticleSystemComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>

// TODO : 몬스터 원거리 공격 구성하기. 발사 로직 구성하기.
AEnemySkill::AEnemySkill()
{
	PrimaryActorTick.bCanEverTick = false;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collsiion"));
	Collision->SetSphereRadius(SkillRadius);
	Collision->SetCollisionProfileName(FName(TEXT("EnemySkill")));
	SetRootComponent(Collision);

	WaterBall = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("WaterBall"));
	ConstructorHelpers::FObjectFinder<UParticleSystem> WaterBallTemp(TEXT("/Script/Engine.ParticleSystem'/Game/Effects/P_EnemySkill.P_EnemySkill'"));
	if (WaterBallTemp.Succeeded())
	{
		WaterBall->SetTemplate(WaterBallTemp.Object);
	}
	WaterBall->SetupAttachment(Collision);

	MoveComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MoveComp"));
	MoveComp->ProjectileGravityScale = 0.f;
	//MoveComp->InitialSpeed = 
}

void AEnemySkill::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemySkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

