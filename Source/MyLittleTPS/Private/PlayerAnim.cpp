// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnim.h"
#include "Characters/TPSPlayer.h"
#include "Components/TPSPlayerFireComponent.h"

#include <GameFramework/PawnMovementComponent.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PlayerAnim)

UPlayerAnim::UPlayerAnim()
{
	ConstructorHelpers::FObjectFinder<UAnimMontage> UpperTemp(TEXT("/Script/Engine.AnimMontage'/Game/Animations/AM_PlayerUpperMontage.AM_PlayerUpperMontage'"));
	if (UpperTemp.Succeeded()) UpperMontage = UpperTemp.Object;
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

void UPlayerAnim::OnEndReload(bool Interruption)
{
	auto Player = Cast<ATPSPlayer>(TryGetPawnOwner());
	if (Player != nullptr)
	{
		if(auto FireComp = Cast<UTPSPlayerFireComponent>(Player->GetFireComp()))
		{
			FireComp->EndReload(Interruption);
		}
	}
}
