// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Enemy.h"
#include "EnemyFSM.h"
#include <Components/SceneComponent.h>
#include <GameFramework/RotatingMovementComponent.h>
#include <Components/SphereComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Particles/ParticleSystem.h>
#include <Particles/ParticleSystemComponent.h>

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = false;

	Center = CreateDefaultSubobject<USceneComponent>(TEXT("Center"));
	SetRootComponent(Center);

	RotatingComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingComponent"));
	RotatingComponent->RotationRate = FRotator(0.f, 230.f, 0.f);
	RotatingComponent->SetUpdatedComponent(RootComponent);

	ConstructorHelpers::FObjectFinder<UParticleSystem> HitEmitterTemp(TEXT("/Script/Engine.ParticleSystem'/Game/Effects/P_ItemHit.P_ItemHit'"));
	if (HitEmitterTemp.Succeeded()) HitEmitter = HitEmitterTemp.Object;

	ConstructorHelpers::FObjectFinder<USoundWave> ItemSoundTemp(TEXT("/Script/Engine.SoundWave'/Game/Effects/Sounds/ItemLoop.ItemLoop'"));
	if (ItemSoundTemp.Succeeded()) ItemSound = ItemSoundTemp.Object;

	ConstructorHelpers::FObjectFinder<USoundWave> HitSoundTemp(TEXT("/Script/Engine.SoundWave'/Game/Effects/Sounds/ItemHit.ItemHit'"));
	if (HitSoundTemp.Succeeded()) HitSound = HitSoundTemp.Object;
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	AddBall();
	if (ItemSound != nullptr) { UGameplayStatics::SpawnSoundAttached(ItemSound, RootComponent, NAME_None, FVector(ForceInit), EAttachLocation::KeepRelativeOffset, true); }
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AItem::AddBall()
{
	if (NowBallCount < 5)
	{
		// ���ο� ������ ���� �� ��ġ�� ���� ���� ���� �� ��������
		TArray<USceneComponent*> ChildArray;
		RootComponent->GetChildrenComponents(true, ChildArray);
		for (auto elem : ChildArray)
		{
			if(elem->GetName().Contains(TEXT("AudioComponent"), ESearchCase::IgnoreCase)) continue; // ������� ������ ����
			elem->DestroyComponent();
		}
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

	auto NewBall = NewObject<UParticleSystemComponent>(this);
	auto ParticleLoad = LoadObject<UParticleSystem>(nullptr, TEXT("/Script/Engine.ParticleSystem'/Game/Effects/P_ItemBall.P_ItemBall'"));
	if (ParticleLoad != nullptr)
	{
		NewBall->SetTemplate(ParticleLoad);
	}
	NewBall->SetupAttachment(NewCollision);
	NewBall->SetRelativeScale3D(FVector(0.8f));
	NewBall->RegisterComponent();
	AddInstanceComponent(NewBall);
}

void AItem::OnAttackDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Enemy = Cast<AEnemy>(OtherActor);
	if (Enemy != nullptr)
	{
		if (HitEmitter != nullptr) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEmitter, Enemy->GetActorLocation());
		if (HitSound != nullptr) UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, Enemy->GetActorLocation());

		Enemy->FSM->OnAttackDamage(BallDamage);
	}
}