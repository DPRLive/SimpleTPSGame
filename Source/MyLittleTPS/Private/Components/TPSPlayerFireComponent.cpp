// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/TPSPlayerFireComponent.h"
#include "Bullet.h"
#include "Characters/TPSPlayer.h"
#include "PlayerAnim.h"

#include <Camera/CameraComponent.h>
#include <Kismet/KismetMathLibrary.h>
#include <Kismet/GameplayStatics.h>
#include <Particles/ParticleSystem.h>
#include <Curves/CurveVector.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(TPSPlayerFireComponent)

UTPSPlayerFireComponent::UTPSPlayerFireComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<UParticleSystem> EmitterTemp(TEXT("/Script/Engine.ParticleSystem'/Game/Effects/P_Shoot.P_Shoot'"));
	if (EmitterTemp.Succeeded()) GunShootEmitter = EmitterTemp.Object;

	ConstructorHelpers::FObjectFinder<USoundWave> FireSoundTemp(TEXT("/Script/Engine.SoundWave'/Game/Effects/Sounds/GunFire.GunFire'"));
	if (FireSoundTemp.Succeeded()) FireSound = FireSoundTemp.Object;

	ConstructorHelpers::FObjectFinder<USoundWave> DryGunSoundTemp(TEXT("/Script/Engine.SoundWave'/Game/Effects/Sounds/DryGun.DryGun'"));
	if (DryGunSoundTemp.Succeeded()) DryGunSound = DryGunSoundTemp.Object;

	ConstructorHelpers::FObjectFinder<USoundWave> SwapAutoFireSoundTemp(TEXT("/Script/Engine.SoundWave'/Game/Effects/Sounds/LightOn.LightOn'"));
	if (SwapAutoFireSoundTemp.Succeeded()) SwapAutoFireSound = SwapAutoFireSoundTemp.Object;
	
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
	RecoilTime += DeltaTime;
	
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

	if(const auto PlayerCameraComp = Player->GetCameraComp())
	{
		// zoom in / zoom out
		float TargetFov = IsZoom ? 60.f : 90.f;
		float NewFov = FMath::FInterpTo(PlayerCameraComp->FieldOfView, TargetFov, DeltaTime, 20.f);
		PlayerCameraComp->FieldOfView = NewFov;
	}
	
	// 반동 넣기
	if (!RecoilValue.IsZero() && RecoilTime >= 0.01f)
	{
		RecoilValue = FMath::Vector2DInterpTo(RecoilValue, FVector2D(0.f), DeltaTime, 12.f);
		Player->AddControllerYawInput(RecoilValue.X);
		Player->AddControllerPitchInput(-RecoilValue.Y);

		RecoilTime = 0.f;
	}
}

void UTPSPlayerFireComponent::BeginPlay()
{
	Super::BeginPlay();

	GunState = EGunState::Idle;
	Mag = MaxMag;

	if(Player != nullptr)
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
	if(Player && Anim->GetUpperMontage() != nullptr)
	{
		Player->PlayAnimMontage(Anim->GetUpperMontage(), 1.5f, TEXT("Reload"));
	}
}

void UTPSPlayerFireComponent::EndReload(const bool Interruption)
{
	//Anim Notify로 장전 모션이 성공적으로 끝나면 실제 총알을 넣어줌
	if(!Interruption) Mag = MaxMag;
	GunState = EGunState::Idle;
}

void UTPSPlayerFireComponent::InputFire()
{
	if (GunState != EGunState::Idle) return;
	if (Mag <= 0)
	{
		if (DryGunSound != nullptr)
		{
			if(const auto PlayerGun = Player->GetGunMesh())
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), DryGunSound, PlayerGun->GetComponentLocation());
			}
		}
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
	if (CurrentTime >= (1 / FirePerSeconds)) // 연사 속도 조절
	{
		if (Mag <= 0) return;
		Mag--;

		const auto PlayerGun = Player->GetGunMesh();
		if(PlayerGun == nullptr) return;
		
		FTransform BulletTrans = PlayerGun->GetSocketTransform(TEXT("FirePosition"));
		if (Player->GetVelocity().Size() > 400.f)
		{
			// 달리면서 쏘면 조준 그런거 안함
			if(Anim != nullptr && Anim->GetUpperMontage() != nullptr)
			{
				Player->PlayAnimMontage(Anim->GetUpperMontage(), 1.0f, TEXT("FireHip"));
			}
		}
		else if(auto PlayerCamera = Player->GetCameraComp())
		{
			if(Anim != nullptr && Anim->GetUpperMontage() != nullptr)
			{
				Player->PlayAnimMontage(Anim->GetUpperMontage(), 1.0f, TEXT("Fire"));
			}
			FVector StartPos = PlayerCamera->GetComponentLocation();
			FVector DestActorPos = StartPos + PlayerCamera->GetForwardVector() * MaxFineSight; // 200m까지는 잘 쏨
			FHitResult Result;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(Player); // 내가 내총에 맞을수는 없다

			// LineTrace를 통해 정조준 가능 거리에 있다면 그쪽으로 정확히 날림
			bool bHit = GetWorld()->LineTraceSingleByChannel(Result, StartPos, DestActorPos, ECollisionChannel::ECC_Visibility, Params);
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

	if (IsZoom) // 줌 상태면 반동 감소
	{
		RecoilValue.X *= 0.5;
		RecoilValue.Y *= 0.5;
	}
}

void UTPSPlayerFireComponent::SwapAutoFire()
{
	IsAutoFire = !IsAutoFire;
	
	if (SwapAutoFireSound != nullptr)
	{
		if(const auto PlayerGun = Player->GetGunMesh())
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), SwapAutoFireSound, PlayerGun->GetComponentLocation());
		}
	}
	
	DelegateSwapFireType.ExecuteIfBound(IsAutoFire);
}

void UTPSPlayerFireComponent::Zoom()
{
	IsZoom = !IsZoom;
}