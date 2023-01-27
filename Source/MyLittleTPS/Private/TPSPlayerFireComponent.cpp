// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayerFireComponent.h"
#include "Bullet.h"
#include "TPSPlayer.h"
#include "PlayerAnim.h"
#include <Camera/CameraComponent.h>
#include <Kismet/KismetMathLibrary.h>

UTPSPlayerFireComponent::UTPSPlayerFireComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
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
	float ReloadTime = Player->PlayAnimMontage(Anim->UpperMontage, 1.0f, TEXT("Reload"));

	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &UTPSPlayerFireComponent::EndReload, ReloadTime, false);
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
	Fire();
	GunState = EGunState::Fire;

	// 타이머 연사
	//Fire();
	//if (IsAutoFire)
	//{
	//	GetWorld()->GetTimerManager().SetTimer(FireTimer, this, 
	//		&UTPSPlayerFireComponent::InputFire, 1 / FirePerSeconds, false);
	//}
}

void UTPSPlayerFireComponent::StopFire()
{
	if (GunState != EGunState::Fire) return; // 발사 상태만 멈출수 있다.
	GunState = EGunState::Idle;
	// 타이머 연사
	// GetWorld()->GetTimerManager().ClearTimer(FireTimer);
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
			FVector DestActorPos = Player->CameraComp->GetForwardVector() * 20000; // 200m까지는 잘 쏨
			FHitResult Result;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(Player); // 내가 내총에 맞을수는 없지

			// 50m 이내에 물체를 조준했다면 거기를 목표로 조준
			bool bHit = GetWorld()->LineTraceSingleByChannel(Result, StartPos, DestActorPos, ECollisionChannel::ECC_Visibility);
			if (bHit) DestActorPos = Result.Location;
			
			// 총을 발사할 전방방향 계산
			BulletTrans.SetRotation(FQuat(UKismetMathLibrary::FindLookAtRotation(BulletTrans.GetLocation(), DestActorPos)));
		}
		GetWorld()->SpawnActor<ABullet>(BulletFactory, BulletTrans);
		CurrentTime = 0;
	}
}

void UTPSPlayerFireComponent::SwapAutoFire()
{
	IsAutoFire = !IsAutoFire;
}

void UTPSPlayerFireComponent::Zoom()
{
	IsZoom = !IsZoom;
}