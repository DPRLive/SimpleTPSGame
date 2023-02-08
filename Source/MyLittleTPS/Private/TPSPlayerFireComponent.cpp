// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayerFireComponent.h"
#include "Bullet.h"
#include "TPSPlayer.h"
#include "PlayerAnim.h"
#include <Camera/CameraComponent.h>
#include <Kismet/KismetMathLibrary.h>
#include <Kismet/GameplayStatics.h>
#include <Particles/ParticleSystem.h>
#include <Curves/CurveVector.h>

UTPSPlayerFireComponent::UTPSPlayerFireComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<UParticleSystem> EmitterTemp(TEXT("/Script/Engine.ParticleSystem'/Game/Effects/P_Shoot.P_Shoot'"));
	if (EmitterTemp.Succeeded()) GunShootEmitter = EmitterTemp.Object;

	ConstructorHelpers::FObjectFinder<USoundWave> FireSoundTemp(TEXT("/Script/Engine.SoundWave'/Game/Effects/Sounds/GunFire.GunFire'"));
	if (FireSoundTemp.Succeeded()) FireSound = FireSoundTemp.Object;

	ConstructorHelpers::FObjectFinder<USoundWave> DryGunSoundTemp(TEXT("/Script/Engine.SoundWave'/Game/Effects/Sounds/DryGun.DryGun'"));
	if (DryGunSoundTemp.Succeeded()) DryGunSound = DryGunSoundTemp.Object;

	ConstructorHelpers::FObjectFinder<UCurveVector> CurveTemp(TEXT("/Script/Engine.CurveVector'/Game/RecoilCurve.RecoilCurve'"));
	if (CurveTemp.Succeeded()) RecoilCurve = CurveTemp.Object;
}

void UTPSPlayerFireComponent::SetupPlayerInput(UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &UTPSPlayerFireComponent::InputFire);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Released, this, &UTPSPlayerFireComponent::StopFire);
	PlayerInputComponent->BindAction(TEXT("SwapAutoFire"), IE_Pressed, this, &UTPSPlayerFireComponent::SwapAutoFire);
	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &UTPSPlayerFireComponent::ReloadState);
	PlayerInputComponent->BindAction(TEXT("Zoom"), IE_Pressed, this, &UTPSPlayerFireComponent::Zoom);
	PlayerInputComponent->BindAction(TEXT("Zoom"), IE_Released, this, &UTPSPlayerFireComponent::Zoom);
}

void UTPSPlayerFireComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CurrentTime += DeltaTime;

	switch (GunState)
	{
	case EGunState::Idle:
		break;
	case EGunState::Fire:
		FireState();
		break;
	case EGunState::Reload:
		ReloadState();
		break;
	}

	// zoom in / zoom out
	float TargetFov = IsZoom ? 60.f : 90.f;
	float NewFov = FMath::FInterpTo(Player->CameraComp->FieldOfView, TargetFov, DeltaTime, 20.f);
	Player->CameraComp->FieldOfView = NewFov;
	
	// 반동 넣기
	if (!RecoilValue.IsZero())
	{
		RecoilValue = FMath::Vector2DInterpTo(RecoilValue, FVector2D(0.f), DeltaTime, 12.f);
		Player->AddControllerYawInput(RecoilValue.X);
		Player->AddControllerPitchInput(-RecoilValue.Y);
	}
}

void UTPSPlayerFireComponent::BeginPlay()
{
	Super::BeginPlay();

	GunState = EGunState::Idle;
	Mag = MaxMag;

	Anim = Cast<UPlayerAnim>(Player->GetMesh()->GetAnimInstance());
}

void UTPSPlayerFireComponent::FireState()
{
	if (IsAutoFire) // 연사면 계속 발사
	{
		Fire();
	}
}

void UTPSPlayerFireComponent::ReloadState()
{
	if (GunState != EGunState::Idle) return;
	GunState = EGunState::Reload;
	// 장전 모션 실행
	Player->PlayAnimMontage(Anim->UpperMontage, 1.0f, TEXT("Reload"));
}

void UTPSPlayerFireComponent::EndReload()
{
	//Anim Notify로 장전 모션 끝나면 장전
	Mag = MaxMag;
	GunState = EGunState::Idle;
}

void UTPSPlayerFireComponent::InputFire()
{
	if (GunState != EGunState::Idle) return;
	if (Mag <= 0)
	{
		if (DryGunSound != nullptr) UGameplayStatics::PlaySoundAtLocation(GetWorld(), DryGunSound, Player->GunMesh->GetComponentLocation());
		return;
	}
	Fire();
	GunState = EGunState::Fire;
}

void UTPSPlayerFireComponent::StopFire()
{
	if (GunState != EGunState::Fire) return; // 발사 상태만 멈출수 있다.
	GunState = EGunState::Idle;
	RecoilCount = 0;
}

void UTPSPlayerFireComponent::Fire()
{
	if (CurrentTime >= (1 / FirePerSeconds))
	{
		if (Mag <= 0) return;
		Mag--;

		FTransform BulletTrans = Player->GunMesh->GetSocketTransform(TEXT("FirePosition"));
		if (Player->GetVelocity().Size() > 400.f)
		{
			// 달리면서 쏘면 조준 그런거 안함
			Player->PlayAnimMontage(Anim->UpperMontage, 1.0f, TEXT("FireHip"));
		}
		else
		{
			Player->PlayAnimMontage(Anim->UpperMontage, 1.0f, TEXT("Fire"));

			FVector StartPos = Player->CameraComp->GetComponentLocation();
			FVector DestActorPos = StartPos + Player->CameraComp->GetForwardVector() * 20000; // 200m까지는 잘 쏨
			FHitResult Result;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(Player); // 내가 내총에 맞을수는 없지

			// 물체를 조준했다면 거기를 목표로 조준
			bool bHit = GetWorld()->LineTraceSingleByChannel(Result, StartPos, DestActorPos, ECollisionChannel::ECC_Visibility);
			if (bHit) DestActorPos = Result.ImpactPoint;

			// 총을 발사할 전방방향 계산
			BulletTrans.SetRotation(FQuat(UKismetMathLibrary::FindLookAtRotation(BulletTrans.GetLocation(), DestActorPos)));
		}

		if(GunShootEmitter != nullptr) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GunShootEmitter, BulletTrans);
		if (FireSound != nullptr) UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, BulletTrans.GetLocation());

		GetWorld()->SpawnActor<ABullet>(BulletFactory, BulletTrans);
		CurrentTime = 0;

		// 반동 추가
		AddRecoil();
	}
}

void UTPSPlayerFireComponent::AddRecoil()
{
	RecoilCount++;
	FVector NewRecoilValue = RecoilCurve->GetVectorValue(RecoilCount);
	RecoilValue.X = NewRecoilValue.X;
	RecoilValue.Y = NewRecoilValue.Y;
}

void UTPSPlayerFireComponent::SwapAutoFire()
{
	IsAutoFire = !IsAutoFire;
}

void UTPSPlayerFireComponent::Zoom()
{
	IsZoom = !IsZoom;
}