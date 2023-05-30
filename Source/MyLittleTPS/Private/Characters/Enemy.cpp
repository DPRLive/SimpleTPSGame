// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Enemy.h"
#include "Components/EnemyFSM.h"
#include "Characters/TPSPlayer.h"
#include "EnemySkill.h"

#include <Components/CapsuleComponent.h>
#include <Components/BoxComponent.h>
#include <Kismet/KismetMathLibrary.h>
#include <Kismet/GameplayStatics.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(Enemy)

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = false;

	auto Collision = Cast<UCapsuleComponent>(RootComponent);
	if (Collision != nullptr)
	{
		Collision->SetCollisionProfileName(TEXT("Enemy"));
	}

	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonKhaimera/Characters/Heroes/Khaimera/Meshes/Khaimera.Khaimera'"));
	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		GetMesh()->SetRelativeScale3D(FVector(0.85f));
		GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90.f), FRotator(0, -90.f, 0));
	}

	AttackAreaL = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackAreaL"));
	AttackAreaL->SetBoxExtent(FVector(5.f, 20.f, 15.f));
	AttackAreaL->SetCollisionProfileName(TEXT("EnemyAttackArea"));
	AttackAreaL->SetupAttachment(GetMesh(), TEXT("AttackAreaL"));

	AttackAreaR = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackAreaR"));
	AttackAreaR->SetBoxExtent(FVector(5.f, 20.f, 15.f));
	AttackAreaR->SetCollisionProfileName(TEXT("EnemyAttackArea"));
	AttackAreaR->SetupAttachment(GetMesh(), TEXT("AttackAreaR"));

	FSM = CreateDefaultSubobject<UEnemyFSM>(TEXT("FSM"));

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned; // 스폰 또는 월드 배치시 AIContoller 자동 pose
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	Hp = MaxHp;
	AttackAreaL->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapAttackArea);
	AttackAreaR->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapAttackArea);
	AttackAreaOff();
	
	if(URORates.Num() <= 0) return;
	
	if(UActorComponent* SKMComp = GetComponentByClass(USkinnedMeshComponent::StaticClass()))
	{
		auto SKMesh =  Cast<USkinnedMeshComponent>(SKMComp);
		SKMesh->bEnableUpdateRateOptimizations = true;
		SKMesh->AnimUpdateRateParams->bShouldUseLodMap = true;
		SKMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;

		for(uint8 i = 0; i < URORates.Num(); i++)
		{
			SKMesh->AnimUpdateRateParams->LODToFrameSkipMap.Add(i, URORates[i]);
		}
	}
}

float AEnemy::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	AttackAreaOff(); // 공격 애니메이션 중에 상태가 바뀌었을 수 있으니 공격 Area 체크 중지
	
	if(Hp - Damage > 0)
	{
		Hp -= Damage;
		FSM->TakeDamage();
	}
	else
	{
		Hp = 0;
		SetActorEnableCollision(false); // 사망 판정 시 Collision OFF
		OnEnemyDie.Broadcast(); // 바인딩 된 사망 후 로직들 실행
	}
	return Damage;
}

AEnemySkill* AEnemy::GetEnemySkillCDO() const
{
	return Cast<AEnemySkill>(EnemySkill.GetDefaultObject());
}

void AEnemy::AttackAreaOn()
{
	AttackAreaL->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AttackAreaR->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::AttackAreaOff()
{
	AttackAreaL->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackAreaR->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::ActivateEnemySkill() // Target의 위치 (플레이어)를 알아내 그 쪽 Rotation으로 스킬 생성
{
	if(IsValid(FSM))
	{
		if(auto Target = FSM->GetTarget())
		{
			FRotator SkillRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation());
			if(EnemySkill != nullptr)
			{
				GetWorld()->SpawnActor<AEnemySkill>(EnemySkill, GetActorLocation(), SkillRotation);
			}
		}
	}
}

void AEnemy::OnOverlapAttackArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Player = Cast<ATPSPlayer>(OtherActor);
	if (Player != nullptr)
	{
		UGameplayStatics::ApplyDamage(Player, EnemyAttackDamage, nullptr, nullptr, nullptr);
	}
}
