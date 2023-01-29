// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include <GameFramework/ProjectileMovementComponent.h>
#include <Components/SphereComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Particles/ParticleSystem.h>
#include "EnemyFSM.h"
#include "Enemy.h"

ABullet::ABullet()
{
	PrimaryActorTick.bCanEverTick = false;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(10);
	// 콜리전 bullet으로 설정
	SphereComp->SetCollisionProfileName(TEXT("Bullet"));
	SphereComp->SetGenerateOverlapEvents(false);
	SphereComp->SetNotifyRigidBodyCollision(true); // 히트 이벤트 생성
	SetRootComponent(SphereComp);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshTemp(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Shapes/Shape_Pipe.Shape_Pipe'"));
	if (MeshTemp.Succeeded())
	{
		MeshComp->SetStaticMesh(MeshTemp.Object);
		MeshComp->SetRelativeScale3D(FVector(0.15, 0.15, 0.15));
		MeshComp->SetRelativeLocationAndRotation(FVector(0, 0, -2.5), FRotator(0, -90, 0));
		// 스태틱 콜리전 비활성화
		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	MeshComp->SetupAttachment(RootComponent);

	MoveComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MoveComp"));
	MoveComp->InitialSpeed = BulletInitSpeed;
	MoveComp->MaxSpeed = BulletMaxSpeed;
	MoveComp->ProjectileGravityScale = 0.2f;
	MoveComp->SetUpdatedComponent(RootComponent);

	ConstructorHelpers::FObjectFinder<UParticleSystem> EmitterTemp(TEXT("/Script/Engine.ParticleSystem'/Game/Effects/P_BulletHit.P_BulletHit'"));
	if (EmitterTemp.Succeeded()) HitEmitter = EmitterTemp.Object;

	ConstructorHelpers::FObjectFinder<UParticleSystem> EnemyHitEmitterTemp(TEXT("/Script/Engine.ParticleSystem'/Game/Effects/P_BulletHitEnemy.P_BulletHitEnemy'"));
	if (EnemyHitEmitterTemp.Succeeded()) EnemyHitEmitter = EnemyHitEmitterTemp.Object;
}

void ABullet::BeginPlay()
{
	Super::BeginPlay();
	// 2초 뒤 삭제
	SetLifeSpan(2.0f);
	SphereComp->OnComponentHit.AddDynamic(this, &ABullet::OnBulletHit);
}

void ABullet::OnBulletHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	auto PresetName = HitComponent->GetCollisionProfileName();
	if (OtherActor != nullptr && PresetName.Compare(FName(TEXT("Enemy")))) // 상대 프리셋이 Enemy면 삭제해버림
	{
		auto Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy != nullptr)
		{
			if (HitEmitter != nullptr) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EnemyHitEmitter, Hit.Location);
			Enemy->FSM->OnAttackDamage(1000);
		}
		else
		{
			if (HitEmitter != nullptr) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEmitter, Hit.Location);
		}
		Destroy();
	}
}