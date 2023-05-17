// Fill out your copyright notice in the Description page of Project Settings.


#include "SKills/EnergyShootSkill.h"
#include "Characters/Enemy.h"
#include "Components/EnemyFSM.h"
#include <Components/BoxComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Particles/ParticleSystem.h>
#include <Particles/ParticleSystemComponent.h>

// Sets default values
AEnergyShootSkill::AEnergyShootSkill()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SkillRangeCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("SkillRangeCollision"));
	SkillRangeCollision->SetCollisionProfileName(TEXT("Skill"));
	SkillRangeCollision->SetBoxExtent(FVector(32.f, 300.f, 50.f));
	SetRootComponent(SkillRangeCollision);

	SkillEmitter = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SkillEmitter"));
	
	ConstructorHelpers::FObjectFinder<UParticleSystem> SkillEmitterTemp(TEXT("/Script/Engine.ParticleSystem'/Game/Effects/P_EnergyShoot.P_EnergyShoot'"));
	if (SkillEmitterTemp.Succeeded()) SkillEmitter->SetTemplate(SkillEmitterTemp.Object);
	
	SkillEmitter->SetRelativeLocationAndRotation(FVector(0.f, -290.f, 0.f), FRotator(0.f, 0.f, 90.f));
	SkillEmitter->SetRelativeScale3D(FVector(0.5f, 0.5f, 1.f));
	SkillEmitter->SetupAttachment(RootComponent);

	ConstructorHelpers::FObjectFinder<USoundWave> SKillUseSoundTemp(TEXT("/Script/Engine.SoundWave'/Game/Effects/Sounds/EnergyShoot.EnergyShoot'"));
	if (SKillUseSoundTemp.Succeeded()) SkillUseSound = SKillUseSoundTemp.Object;

	ConstructorHelpers::FObjectFinder<USoundWave> SKillLoopSoundTemp(TEXT("/Script/Engine.SoundWave'/Game/Effects/Sounds/EnergyShootLoop.EnergyShootLoop'"));
	if (SKillLoopSoundTemp.Succeeded()) SkillLoopSound = SKillLoopSoundTemp.Object;

	ConstructorHelpers::FObjectFinder<UParticleSystem> SkillHitEmitterTemp(TEXT("/Script/Engine.ParticleSystem'/Game/Effects/P_EnergyShootHit.P_EnergyShootHit'"));
	if (SkillHitEmitterTemp.Succeeded()) SkillHitEmitter = SkillHitEmitterTemp.Object;

	ConstructorHelpers::FObjectFinder<USoundWave> SKillHitSoundTemp(TEXT("/Script/Engine.SoundWave'/Game/Effects/Sounds/EnergyShootHit.EnergyShootHit'"));
	if (SKillHitSoundTemp.Succeeded()) SkillHitSound = SKillHitSoundTemp.Object;

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

	if (SkillUseSound != nullptr) UGameplayStatics::PlaySoundAtLocation(GetWorld(), SkillUseSound, GetActorLocation());
	if (SkillLoopSound != nullptr) UGameplayStatics::SpawnSoundAttached(SkillLoopSound, SkillEmitter, NAME_None, FVector(ForceInit), FRotator(ForceInit), EAttachLocation::KeepRelativeOffset, true);

}

void AEnergyShootSkill::OnBeginOverlapEnemy(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Enemy = Cast<AEnemy>(OtherActor);
	if (Enemy != nullptr)
	{
		if (SkillHitEmitter != nullptr) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SkillHitEmitter, Enemy->GetActorLocation());
		if (SkillHitSound != nullptr) UGameplayStatics::PlaySoundAtLocation(GetWorld(), SkillHitSound, Enemy->GetActorLocation());
		UGameplayStatics::ApplyDamage(Enemy, 1000.f, nullptr, nullptr, nullptr);
	}
}