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
	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Released, this, &UTPSPlayerFireComponent::Reload);
}

void UTPSPlayerFireComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	CurrentTime += DeltaTime;
	if (IsAutoFire && (FireState == EFireState::Fire))
	{
		if (CurrentTime >= (1 / FirePerSeconds))
		{
			Fire();
			CurrentTime = 0;
		}
	}
}

void UTPSPlayerFireComponent::BeginPlay()
{
	Super::BeginPlay();
	Mag = MaxMag;
}

void UTPSPlayerFireComponent::InputFire()
{
	if (FireState != EFireState::Idle) return;
	if (CurrentTime >= (1 / FirePerSeconds))
	{
		Fire();
		CurrentTime = 0;
		FireState = EFireState::Fire;
	}
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
	FireState = EFireState::Idle;
	// 타이머 연사
	// GetWorld()->GetTimerManager().ClearTimer(FireTimer);
}

void UTPSPlayerFireComponent::Fire()
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
}

void UTPSPlayerFireComponent::SwapAutoFire()
{
	IsAutoFire = !IsAutoFire;
}

void UTPSPlayerFireComponent::Reload()
{
	FireState = EFireState::Reload;
	// Reload Animation Play
	// 임시, 추후 Anim Notify로 장전 모션 끝나면 장전 시킬 예정
	Mag = MaxMag;
	FireState = EFireState::Idle;
}
