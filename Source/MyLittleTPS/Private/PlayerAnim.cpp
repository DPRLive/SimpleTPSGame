// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnim.h"
#include "TPSPlayer.h"
#include <GameFramework/PawnMovementComponent.h>
#include <Camera/CameraComponent.h>
#include <Kismet/KismetMathLibrary.h>

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
		UE_LOG(LogTemp, Warning, TEXT("%f"), UpDownRotate);

		auto Movement = Player->GetMovementComponent();
		if (Movement != nullptr)
		{
			IsInAir = Movement->IsFalling();
		}
	}
}