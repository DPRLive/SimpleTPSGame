// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnim.h"
#include "TPSPlayer.h"
#include "TPSPlayerFireComponent.h"
#include <GameFramework/PawnMovementComponent.h>
#include <Camera/CameraComponent.h>
#include <Kismet/KismetMathLibrary.h>

UPlayerAnim::UPlayerAnim()
{
	ConstructorHelpers::FObjectFinder<UAnimMontage> UpperTemp(TEXT("/Script/Engine.AnimMontage'/Game/Animations/AM_PlayerUpperMontage.AM_PlayerUpperMontage'"));
	if (UpperTemp.Succeeded()) UpperMontage = UpperTemp.Object;

	ConstructorHelpers::FObjectFinder<UAnimMontage> FullTemp(TEXT("/Script/Engine.AnimMontage'/Game/Animations/AM_PlayerFullBodyMontage.AM_PlayerFullBodyMontage'"));
	if (FullTemp.Succeeded()) FullMontage = FullTemp.Object;
}

void UPlayerAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Player = Cast<ATPSPlayer>(TryGetPawnOwner());
	if (Player != nullptr)
	{
		RSpeed = FVector::DotProduct(Player->GetVelocity(), Player->GetActorRightVector());
		FSpeed = FVector::DotProduct(Player->GetVelocity(), Player->GetActorForwardVector());
		JumpCount = Player->JumpCurrentCount;

		auto Controller = Player->GetController();
		if (Controller != nullptr) UpDownRotate = -Controller->GetControlRotation().Pitch;
		else UpDownRotate = 0.f;

		auto Movement = Player->GetMovementComponent();
		if (Movement != nullptr)
		{
			IsInAir = Movement->IsFalling();
		}
	}
}

void UPlayerAnim::OnEndReload()
{
	auto Player = Cast<ATPSPlayer>(TryGetPawnOwner());
	if (Player != nullptr)
	{
		auto FireComp = Cast<UTPSPlayerFireComponent>(Player->FireComp);
		FireComp->EndReload();
	}
}
