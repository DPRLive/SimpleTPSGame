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
	if (IsAutoFire) // ����� ��� �߻�
	{
		Fire();
	}
}

void UTPSPlayerFireComponent::InputFire()
{
	if (GunState != EGunState::Idle) return;
	Fire();
	GunState = EGunState::Fire;

	// Ÿ�̸� ����
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
	// Ÿ�̸� ����
	// GetWorld()->GetTimerManager().ClearTimer(FireTimer);
}

void UTPSPlayerFireComponent::Fire()
{
	if (CurrentTime >= (1 / FirePerSeconds))
	{
		if (Mag <= 0) return;
		Mag--;

		FVector StartPos = Player->CameraComp->GetComponentLocation();
		FVector DestActorPos = Player->CameraComp->GetForwardVector() * 15000; // 150m������ �� ��
		FHitResult Result;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Player); // ���� ���ѿ� �������� ����

		// 50m �̳��� ��ü�� �����ߴٸ� �ű⸦ ��ǥ�� ����
		bool bHit = GetWorld()->LineTraceSingleByChannel(Result, StartPos, DestActorPos, ECollisionChannel::ECC_Visibility);
		if (bHit)
		{
			DestActorPos = Result.Location;
		}

		// ���� �߻��� ������� ���
		FTransform BulletTrans = Player->GunMesh->GetSocketTransform(TEXT("FirePosition"));
		if (FVector::Distance(StartPos, DestActorPos) > 1000.f) // �ʹ� ������ ��� �̻��ϴϱ� ����ﶩ �׳� �ѱ� ���� �������� �߻�
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
	// Reload Animation Play ������� �������� üũ �ʿ���
	// �ӽ�, ���� Anim Notify�� ���� ��� ������ ���� ��ų ����,
	Mag = MaxMag;
	GunState = EGunState::Idle;
}
