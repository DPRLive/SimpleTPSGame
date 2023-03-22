// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/EnergyBombSkill.h"
#include "Characters/Enemy.h"
#include "Components/EnemyFSM.h"
#include <Components/SphereComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Particles/ParticleSystem.h>

// Sets default values
AEnergyBombSkill::AEnergyBombSkill()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SkillRangeCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SkillRangeCollision"));
	SkillRangeCollision->SetSphereRadius(1000.f);
	SkillRangeCollision->SetCollisionProfileName(TEXT("Skill"));
	SetRootComponent(SkillRangeCollision);

	ConstructorHelpers::FObjectFinder<UParticleSystem> SkillEmitterTemp(TEXT("/Script/Engine.ParticleSystem'/Game/Effects/P_EnergyBoom.P_EnergyBoom'"));
	if (SkillEmitterTemp.Succeeded()) SkillEmitter = SkillEmitterTemp.Object;

	ConstructorHelpers::FObjectFinder<UParticleSystem> HitEmitterTemp(TEXT("/Script/Engine.ParticleSystem'/Game/Effects/P_EnergyBombHit.P_EnergyBombHit'"));
	if (HitEmitterTemp.Succeeded()) HitEmitter = HitEmitterTemp.Object;

	ConstructorHelpers::FObjectFinder<USoundWave> SKillSoundTemp(TEXT("/Script/Engine.SoundWave'/Game/Effects/Sounds/EnergyBomb.EnergyBomb'"));
	if (SKillSoundTemp.Succeeded()) SkillSound = SKillSoundTemp.Object;

	ConstructorHelpers::FObjectFinder<USoundWave> HitSoundTemp(TEXT("/Script/Engine.SoundWave'/Game/Effects/Sounds/EnergyBombHit.EnergyBombHit'"));
	if (HitSoundTemp.Succeeded()) HitSound = HitSoundTemp.Object;
}

// Called when the game starts or when spawned
void AEnergyBombSkill::BeginPlay()
{
	Super::BeginPlay();
	SkillRangeCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnergyBombSkill::OnBeginOverlapEnemy);

	// 이미터 & 사운드 재생
	if (SkillEmitter != nullptr) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SkillEmitter, GetActorLocation());
	if (SkillSound != nullptr) UGameplayStatics::PlaySoundAtLocation(GetWorld(), SkillSound, GetActorLocation());

	FTimerHandle DestroyTimer;
	GetWorld()->GetTimerManager().SetTimer(DestroyTimer, FTimerDelegate::CreateLambda([this]() { Destroy(); }),
		0.1f, false);
}

void AEnergyBombSkill::OnBeginOverlapEnemy(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Enemy = Cast<AEnemy>(OtherActor);
	if (Enemy != nullptr)
	{
		if (HitEmitter != nullptr) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEmitter, Enemy->GetActorLocation(), FRotator(ForceInit), FVector(2.f));
		if (HitSound != nullptr) UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, Enemy->GetActorLocation());
		Enemy->FSM->OnAttackDamage(1000.f);
	}
}
