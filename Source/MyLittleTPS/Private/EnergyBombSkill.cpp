// Fill out your copyright notice in the Description page of Project Settings.


#include "EnergyBombSkill.h"
#include "Enemy.h"
#include "EnemyFSM.h"
#include <Components/SphereComponent.h>

// Sets default values
AEnergyBombSkill::AEnergyBombSkill()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SkillRangeCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SkillRangeCollision"));
	SkillRangeCollision->SetSphereRadius(1000.f);
	SkillRangeCollision->SetCollisionProfileName(TEXT("Skill"));

	SetRootComponent(SkillRangeCollision);
}

// Called when the game starts or when spawned
void AEnergyBombSkill::BeginPlay()
{
	Super::BeginPlay();
	SkillRangeCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnergyBombSkill::OnBeginOverlapEnemy);

	// 이미터 & 사운드 재생
	FTimerHandle DestroyTimer;
	GetWorld()->GetTimerManager().SetTimer(DestroyTimer, FTimerDelegate::CreateLambda([this]() { Destroy(); }),
		0.1f, false);
}

void AEnergyBombSkill::OnBeginOverlapEnemy(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr)
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
}
