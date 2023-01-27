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
	if (IsAutoFire) // ����� ��� �߻�
	{
		Fire();
	}
}

void UTPSPlayerFireComponent::ReloadState()
{
	if (GunState != EGunState::Idle) return;
	GunState = EGunState::Reload;
	// ���� ��� ����
	float ReloadTime = Player->PlayAnimMontage(Anim->UpperMontage, 1.0f, TEXT("Reload"));

	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &UTPSPlayerFireComponent::EndReload, ReloadTime, false);
}

void UTPSPlayerFireComponent::EndReload()
{
	//Anim Notify�� ���� ��� ������ ����
	Mag = MaxMag;
	GunState = EGunState::Idle;
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
	if (GunState != EGunState::Fire) return; // �߻� ���¸� ����� �ִ�.
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

		FTransform BulletTrans = Player->GunMesh->GetSocketTransform(TEXT("FirePosition"));
		if (Player->GetVelocity().Size() > 400.f)
		{
			// �޸��鼭 ��� ���� �׷��� ����
			Player->PlayAnimMontage(Anim->UpperMontage, 1.0f, TEXT("FireHip"));
		}
		else
		{
			Player->PlayAnimMontage(Anim->UpperMontage, 1.0f, TEXT("Fire"));

			FVector StartPos = Player->CameraComp->GetComponentLocation();
			FVector DestActorPos = Player->CameraComp->GetForwardVector() * 20000; // 200m������ �� ��
			FHitResult Result;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(Player); // ���� ���ѿ� �������� ����

			// 50m �̳��� ��ü�� �����ߴٸ� �ű⸦ ��ǥ�� ����
			bool bHit = GetWorld()->LineTraceSingleByChannel(Result, StartPos, DestActorPos, ECollisionChannel::ECC_Visibility);
			if (bHit) DestActorPos = Result.Location;
			
			// ���� �߻��� ������� ���
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