// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySkill.h"
#include "Characters/TPSPlayer.h"
#include "Kismet/GameplayStatics.h"

#include <Components/SphereComponent.h>
#include <Particles/ParticleSystem.h>
#include <Particles/ParticleSystemComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(EnemySkill)

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
	SetLifeSpan(20.f);
	Collision->OnComponentHit.AddDynamic(this, &AEnemySkill::OnSkillHit);
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AEnemySkill::OnSkillOverlap);
}

void AEnemySkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemySkill::OnSkillHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	auto Target = Cast<ATPSPlayer>(OtherActor);
	if(IsValid(Target))
	{
		UGameplayStatics::ApplyDamage(Target, SkillDamage, nullptr, nullptr, nullptr);
	}
	if(IsValid(EnemyHitEmitter)) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EnemyHitEmitter, Hit.ImpactPoint);
	Destroy();
}

void AEnemySkill::OnSkillOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(IsValid(EnemyHitEmitter))
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EnemyHitEmitter, GetActorLocation());
	Destroy();
}

