// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Enemy.h"
#include "EnemyFSM.h"
#include <Components/SceneComponent.h>
#include <GameFramework/RotatingMovementComponent.h>
#include <Components/SphereComponent.h>
#include <components/StaticMeshComponent.h>

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = false;

	Center = CreateDefaultSubobject<USceneComponent>(TEXT("Center"));
	SetRootComponent(Center);

	RotatingComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingComponent"));
	RotatingComponent->RotationRate = FRotator(0.f, 230.f, 0.f);
	RotatingComponent->SetUpdatedComponent(RootComponent);
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	AddBall();
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AItem::AddBall()
{
	if (NowBallCount >= 5) return;
	
	// ���ο� ������ ���� �� ��ġ�� ���� ���� ���� �� ��������
	TArray<USceneComponent*> ChildArray;
	RootComponent->GetChildrenComponents(true, ChildArray);
	for (auto elem : ChildArray) elem->DestroyComponent();
	
	NowBallCount++;
	FVector Forward = GetActorForwardVector().GetSafeNormal();

	// �� ������ ������ �� ���� ������ ���� ����
	float Angle;
	if (NowBallCount == 0) Angle = 0;
	else Angle = 360.f / NowBallCount;

	for (uint8 i = 0; i < NowBallCount; i++)
	{
		// ȸ�� �����ִ� �Լ�. �𸮾� �ְ�^^
		FVector SpawnLocation = Forward.RotateAngleAxis(Angle * i, FVector::UpVector) * RangeRadius;
		CreateNewBall(SpawnLocation);
	}

	// 15�� �Ŀ� �ı��� �ǵ�, �� ���� �� ������ �� �߰��ǰ� �ٽ� 15�� ī����
	if(GetWorld()->GetTimerManager().IsTimerActive(DestroyTimer))
	{ 
		GetWorld()->GetTimerManager().ClearTimer(DestroyTimer);
	}
	GetWorld()->GetTimerManager().SetTimer(DestroyTimer,
		FTimerDelegate::CreateLambda([this]() {Destroy(); }), 15.f, false);
}

void AItem::CreateNewBall(FVector& Location)
{
	auto NewCollision = NewObject<USphereComponent>(this);
	NewCollision->SetupAttachment(RootComponent);
	NewCollision->SetSphereRadius(26.f);
	NewCollision->SetRelativeLocation(Location);
	NewCollision->SetCollisionProfileName(TEXT("Skill"));
	NewCollision->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnAttackDamage);
	NewCollision->RegisterComponent();
	AddInstanceComponent(NewCollision);

	auto NewBall = NewObject<UStaticMeshComponent>(this);
	auto Mesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if (Mesh != nullptr)
	{
		NewBall->SetStaticMesh(Mesh);
	}
	NewBall->SetupAttachment(NewCollision);
	NewBall->SetRelativeScale3D(FVector(0.5f));
	NewBall->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	NewBall->RegisterComponent();
	AddInstanceComponent(NewBall);
}

void AItem::OnAttackDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OverlappedComponent->GetCollisionProfileName().Compare(FName(TEXT("Enemy"))))
	{
		auto Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy != nullptr)
		{
			Enemy->FSM->OnAttackDamage(BallDamage);
		}
	}
}