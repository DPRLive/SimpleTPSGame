// Fill out your copyright notice in the Description page of Project Settings.


#include "EnergyShootSkill.h"
#include "Enemy.h"
#include "EnemyFSM.h"
#include <Components/BoxComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>

// Sets default values
AEnergyShootSkill::AEnergyShootSkill()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SkillRangeCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("SkillRangeCollision"));
	SkillRangeCollision->SetCollisionProfileName(TEXT("Skill"));
	SkillRangeCollision->SetBoxExtent(FVector(32.f, 300.f, 32.f));
	SetRootComponent(SkillRangeCollision);

	MoveComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MoveComp"));
	MoveComp->ProjectileGravityScale = 0.f;
	MoveComp->InitialSpeed = 1000.f;
	MoveComp->SetUpdatedComponent(RootComponent);
}

// Called when the game starts or when spawned
void AEnergyShootSkill::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(8.f);
	SkillRangeCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnergyShootSkill::OnBeginOverlapEnemy);
}

void AEnergyShootSkill::OnBeginOverlapEnemy(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OverlappedComponent->GetCollisionProfileName().Compare(FName(TEXT("Enemy"))))
	{
		auto Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy != nullptr)
		{
			Enemy->FSM->OnAttackDamage(1000.f);
		}
	}
}