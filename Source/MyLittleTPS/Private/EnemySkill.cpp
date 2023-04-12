// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySkill.h"
#include "Characters/TPSPlayer.h"
#include <Components/SphereComponent.h>
#include <Particles/ParticleSystem.h>
#include <Particles/ParticleSystemComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>

#include "Kismet/GameplayStatics.h"

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
	MoveComp->InitialSpeed = SkillSpeed;
	MoveComp->MaxSpeed = SkillSpeed;
}

void AEnemySkill::BeginPlay()
{
	Super::BeginPlay();
	Collision->OnComponentHit.AddDynamic(this, &AEnemySkill::OnEnemySkillHit);
}

void AEnemySkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemySkill::OnEnemySkillHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	auto Target = Cast<ATPSPlayer>(OtherActor);
	if(IsValid(Target))
	{
		Target->OnAttackDamage(SkillDamage);
	}
	if(IsValid(EnemyHitEmitter)) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EnemyHitEmitter, Hit.ImpactPoint);
	Destroy();
}

