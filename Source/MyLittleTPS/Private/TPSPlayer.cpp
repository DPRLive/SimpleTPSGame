// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayer.h"
#include "TPSPlayerMoveComponent.h"
#include "TPSPlayerFireComponent.h"
#include "TPSPlayerSkillComponent.h"
#include "PlayerAnim.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Components/CapsuleComponent.h>
#include <Components/SpotLightComponent.h>

ATPSPlayer::ATPSPlayer()
{
	// ������Ʈ�� �����ϰŶ� false
	PrimaryActorTick.bCanEverTick = false;

	auto Collision = Cast<UCapsuleComponent>(RootComponent);
	if (Collision != nullptr)
	{
		Collision->SetCollisionProfileName(TEXT("Player"));
	}

	// SpringArmComponent ����
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

	// ���̷�Ż �޽� ����
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Manny.SKM_Manny'"));
	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	// �� �޽� ����
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	if (TempGunMesh.Succeeded())
	{
		GunMesh->SetSkeletalMesh(TempGunMesh.Object);
		GunMesh->SetupAttachment(GetMesh(), TEXT("hand_r_Socket"));
	}

	GunLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("GunLight"));
	GunLight->SetIntensity(50000.f);
	GunLight->AttenuationRadius = 4000.f;
	GunLight->InnerConeAngle = 15.f;
	GunLight->OuterConeAngle = 25.f;
	GunLight->SetVisibility(false);
	GunLight->SetupAttachment(GunMesh, TEXT("LightPosition"));

	// ����Ʈ ����
	ConstructorHelpers::FObjectFinder<USoundWave> LightOnSoundTemp(TEXT("/Script/Engine.SoundWave'/Game/Effects/Sounds/LightOn.LightOn'"));
	if (LightOnSoundTemp.Succeeded()) LightOnSound = LightOnSoundTemp.Object;

	ConstructorHelpers::FObjectFinder<USoundWave> LightOffSoundTemp(TEXT("/Script/Engine.SoundWave'/Game/Effects/Sounds/LightOff.LightOff'"));
	if (LightOffSoundTemp.Succeeded()) LightOffSound = LightOffSoundTemp.Object;

	// �̵� ��� ������Ʈ ����
	MoveComp = CreateDefaultSubobject<UTPSPlayerMoveComponent>(TEXT("MoveComp"));
	// �Ѿ� �߻� ��� ������Ʈ ����
	FireComp = CreateDefaultSubobject<UTPSPlayerFireComponent>(TEXT("FireComp"));
	// ��ų ��� ������Ʈ ����
	SkillComp = CreateDefaultSubobject<UTPSPlayerSkillComponent>(TEXT("SkillComp"));

	// ���� �ι��� ^^
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
	// ��������Ʈ ȣ��
	InputBindingDelegate.Broadcast(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("LightToggle"), IE_Pressed, this, &ATPSPlayer::LightToggle);
}

void ATPSPlayer::OnAttackDamage(float Damage)
{
	auto Anim = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
	if (Hp - Damage <= 0)
	{
		Hp = 0;
		Controller->UnPossess();
		SetActorEnableCollision(false);
		if(Anim != nullptr) PlayAnimMontage(Anim->FullMontage, 1.0f, FName(TEXT("Death")));
	}
	else
	{
		Hp -= Damage;
		if (Anim != nullptr) PlayAnimMontage(Anim->UpperMontage, 1.2f, FName(TEXT("Hit")));
	}
}

void ATPSPlayer::LightToggle()
{
	bLightOn = !bLightOn;

	if (bLightOn && (LightOnSound != nullptr)) UGameplayStatics::PlaySoundAtLocation(GetWorld(), LightOnSound, GunMesh->GetSocketLocation(TEXT("LightPosition")));
	else if (!bLightOn && (LightOffSound != nullptr)) UGameplayStatics::PlaySoundAtLocation(GetWorld(), LightOffSound, GunMesh->GetSocketLocation(TEXT("LightPosition")));

	GunLight->SetVisibility(bLightOn);
}
