// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/TPSPlayer.h"
#include "Components/TPSPlayerMoveComponent.h"
#include "Components/TPSPlayerFireComponent.h"
#include "Components/TPSPlayerSkillComponent.h"
#include "PlayerAnim.h"

#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Components/CapsuleComponent.h>
#include <Components/SpotLightComponent.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(TPSPlayer)

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
		GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
	}

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	// 건 메시 설정
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

	// 라이트 사운드
	ConstructorHelpers::FObjectFinder<USoundWave> LightOnSoundTemp(TEXT("/Script/Engine.SoundWave'/Game/Effects/Sounds/LightOn.LightOn'"));
	if (LightOnSoundTemp.Succeeded()) LightOnSound = LightOnSoundTemp.Object;

	ConstructorHelpers::FObjectFinder<USoundWave> LightOffSoundTemp(TEXT("/Script/Engine.SoundWave'/Game/Effects/Sounds/LightOff.LightOff'"));
	if (LightOffSoundTemp.Succeeded()) LightOffSound = LightOffSoundTemp.Object;

	// 이동 담당 컴포넌트 소유
	MoveComp = CreateDefaultSubobject<UTPSPlayerMoveComponent>(TEXT("MoveComp"));
	// 총알 발사 담당 컴포넌트 소유
	FireComp = CreateDefaultSubobject<UTPSPlayerFireComponent>(TEXT("FireComp"));
	// 스킬 담당 컴포넌트 소유
	PlayerSkillComp = CreateDefaultSubobject<UTPSPlayerSkillComponent>(TEXT("PlayerSkillComp"));

	// 점프 두번
	JumpMaxCount = 2;
}

void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	Hp = MaxHp;
}

void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// 델리게이트 호출
	DelegateInputBinding.Broadcast(PlayerInputComponent);
	PlayerInputComponent->BindAction(TEXT("LightToggle"), IE_Pressed, this, &ATPSPlayer::LightToggle);
}

void ATPSPlayer::LightToggle()
{
	bLightOn = !bLightOn;

	if (bLightOn && (LightOnSound != nullptr)) UGameplayStatics::PlaySoundAtLocation(GetWorld(), LightOnSound, GunMesh->GetSocketLocation(TEXT("LightPosition")));
	else if (!bLightOn && (LightOffSound != nullptr)) UGameplayStatics::PlaySoundAtLocation(GetWorld(), LightOffSound, GunMesh->GetSocketLocation(TEXT("LightPosition")));

	GunLight->SetVisibility(bLightOn);
}

float ATPSPlayer::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	auto Anim = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
	if (Hp - Damage <= 0)
	{
		Hp = 0;
		
		SetActorEnableCollision(false);
		Controller->UnPossess();

		// 사망시 Ragdoll로 전환
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetSimulatePhysics(true);

		// 죽음을 알림
		DelegateCharacterDie.ExecuteIfBound();
	}
	else
	{
		Hp -= Damage;
		if (Anim != nullptr && Anim->GetUpperMontage() != nullptr) PlayAnimMontage(Anim->GetUpperMontage(), 1.2f, FName(TEXT("Hit")));
	}

	return Damage;
}

UTPSPlayerFireComponent* ATPSPlayer::GetFireComp() const
{
	return Cast<UTPSPlayerFireComponent>(FireComp); 
}

UTPSPlayerSkillComponent* ATPSPlayer::GetSkillComp() const
{
	return Cast<UTPSPlayerSkillComponent>(PlayerSkillComp); 
}

void ATPSPlayer::AddHealth(const float InHp)
{
	if(Hp + InHp > MaxHp) Hp = MaxHp;
	else Hp += InHp;
}
