// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include <GameFramework/ProjectileMovementComponent.h>
#include <Components/SphereComponent.h>
#include <Components/StaticMeshComponent.h>
#include "EnemyFSM.h"
#include "Enemy.h"

ABullet::ABullet()
{
	PrimaryActorTick.bCanEverTick = false;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(10);
	// 콜리전 bullet으로 설정
	SphereComp->SetCollisionProfileName(TEXT("Bullet"));
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

}

void ABullet::BeginPlay()
{
	Super::BeginPlay();
	// 2초 뒤 삭제
	SetLifeSpan(2.0f);
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OnBulletBeginOverlap);
}

void ABullet::OnBulletBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr)
	{
		auto PresetName = OverlappedComponent->GetCollisionProfileName();
		
		if (PresetName.Compare(FName(TEXT("Enemy")))) // 상대 프리셋이 Enemy면 삭제해버림
		{
			auto Enemy = Cast<AEnemy>(OtherActor);
			if (Enemy != nullptr)
			{
				Enemy->FSM->OnAttackDamage(1000);
			}
		}
		Destroy();
	}
}