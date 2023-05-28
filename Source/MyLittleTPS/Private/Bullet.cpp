// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "Characters/Enemy.h"

#include <GameFramework/ProjectileMovementComponent.h>
#include <Components/BoxComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Particles/ParticleSystem.h>
#include <Materials/Material.h>
#include <Kismet/KismetMathLibrary.h>
#include <Components/DecalComponent.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(Bullet)

ABullet::ABullet()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetBoxExtent(FVector(8.f, 1.f, 2.f));
	// 콜리전 bullet으로 설정
	BoxComp->SetCollisionProfileName(TEXT("Bullet"));
	BoxComp->SetGenerateOverlapEvents(false);
	BoxComp->SetNotifyRigidBodyCollision(true); // 히트 이벤트 생성
	SetRootComponent(BoxComp);

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
	MoveComp->ProjectileGravityScale = 1.f;
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
	BoxComp->OnComponentHit.AddDynamic(this, &ABullet::OnBulletHit);
}

void ABullet::OnBulletHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	auto Enemy = Cast<AEnemy>(OtherActor); // 상대가 Enemy면 삭제해버림
	if (Enemy != nullptr)
	{
		if (HitEmitter != nullptr) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EnemyHitEmitter, Hit.Location);
		UGameplayStatics::ApplyDamage(Enemy, BulletDamage, nullptr, nullptr, nullptr);
	}
	else
	{
		if (HitEmitter != nullptr) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEmitter, Hit.Location);
		auto Decal = UGameplayStatics::SpawnDecalAttached(BulletHole, FVector(5.f, 5.f, 5.f), OtherComp, NAME_None, Hit.ImpactPoint,
			UKismetMathLibrary::Conv_VectorToRotator(Hit.ImpactNormal), EAttachLocation::KeepWorldPosition, 5.0f);

		Decal->SetFadeScreenSize(0.001f);
	}
	Destroy();
}