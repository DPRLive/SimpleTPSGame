// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayerFireComponent.h"
#include "Bullet.h"
#include "TPSPlayer.h"
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
	PlayerInputComponent->BindAction(TEXT("SwapAutoFire"), IE_Released, this, &UTPSPlayerFireComponent::SwapAutoFire);
	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Released, this, &UTPSPlayerFireComponent::ReloadState);
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
}

void UTPSPlayerFireComponent::BeginPlay()
{
	Super::BeginPlay();

	GunState = EGunState::Idle;
	Mag = MaxMag;
}

void UTPSPlayerFireComponent::FireState()
{
	if (IsAutoFire) // 연사면 계속 발사
	{
		Fire();
	}
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

		FVector StartPos = Player->CameraComp->GetComponentLocation();
		FVector DestActorPos = Player->CameraComp->GetForwardVector() * 15000; // 150m까지는 잘 쏨
		FHitResult Result;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Player); // 내가 내총에 맞을수는 없지

		// 50m 이내에 물체를 조준했다면 거기를 목표로 조준
		bool bHit = GetWorld()->LineTraceSingleByChannel(Result, StartPos, DestActorPos, ECollisionChannel::ECC_Visibility);
		if (bHit)
		{
			DestActorPos = Result.Location;
		}

		// 총을 발사할 전방방향 계산
		FTransform BulletTrans = Player->GunMesh->GetSocketTransform(TEXT("FirePosition"));
		if (FVector::Distance(StartPos, DestActorPos) > 1000.f) // 너무 가까이 쏘면 이상하니까 가까울땐 그냥 총구 전방 방향으로 발사
			BulletTrans.SetRotation(FQuat(UKismetMathLibrary::FindLookAtRotation(BulletTrans.GetLocation(), DestActorPos)));
		GetWorld()->SpawnActor<ABullet>(BulletFactory, BulletTrans);
		CurrentTime = 0;
	}
}

void UTPSPlayerFireComponent::SwapAutoFire()
{
	IsAutoFire = !IsAutoFire;
}

void UTPSPlayerFireComponent::ReloadState()
{
	if (GunState != EGunState::Idle) return;
	GunState = EGunState::Reload;
	// Reload Animation Play 장전모션 끝났는지 체크 필요함
	// 임시, 추후 Anim Notify로 장전 모션 끝나면 장전 시킬 예정,
	Mag = MaxMag;
	GunState = EGunState::Idle;
}
