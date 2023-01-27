// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayer.h"
#include "TPSPlayerMoveComponent.h"
#include "TPSPlayerFireComponent.h"
#include "TPSPlayerSkillComponent.h"
#include "PlayerAnim.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <Components/CapsuleComponent.h>

ATPSPlayer::ATPSPlayer()
{
	// 컴포넌트로 움직일거라서 false
	PrimaryActorTick.bCanEverTick = false;

	auto Collision = Cast<UCapsuleComponent>(RootComponent);
	if (Collision != nullptr)
	{
		Collision->SetCollisionProfileName(TEXT("Player"));
	}

	// SpringArmComponent 설정
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->TargetArmLength = 200;
	SpringArmComp->SetRelativeLocation(FVector(0, 100, 100));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->bEnableCameraLag = true;
	SpringArmComp->CameraLagSpeed = 15;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->bUsePawnControlRotation = true;
	bUseControllerRotationYaw = true;

	// 스켈레탈 메시 설정
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Manny.SKM_Manny'"));
	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	// 건 메시 설정
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	if (TempGunMesh.Succeeded())
	{
		GunMesh->SetSkeletalMesh(TempGunMesh.Object);
		GunMesh->SetupAttachment(GetMesh(), TEXT("hand_r_Socket"));
	}

	// 이동 담당 컴포넌트 소유
	MoveComp = CreateDefaultSubobject<UTPSPlayerMoveComponent>(TEXT("MoveComp"));
	// 총알 발사 담당 컴포넌트 소유
	FireComp = CreateDefaultSubobject<UTPSPlayerFireComponent>(TEXT("FireComp"));
	// 스킬 담당 컴포넌트 소유
	SkillComp = CreateDefaultSubobject<UTPSPlayerSkillComponent>(TEXT("SkillComp"));

	// 점프 두번해 ^^
	JumpMaxCount = 2;
}

void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	Hp = MaxHp;
}

void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// 델리게이트 호출
	InputBindingDelegate.Broadcast(PlayerInputComponent);
}

void ATPSPlayer::OnAttackDamage(float Damage)
{
	auto Anim = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
	if (Hp - Damage < 0)
	{
		Hp = 0;
		if(Anim != nullptr) PlayAnimMontage(Anim->FullMontage, 1.0f, FName(TEXT("Death")));
	}
	else
	{
		Hp -= Damage;
		if (Anim != nullptr) PlayAnimMontage(Anim->FullMontage, 1.5f, FName(TEXT("Hit")));
	}
}