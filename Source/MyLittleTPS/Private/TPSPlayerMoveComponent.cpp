// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayerMoveComponent.h"
#include "TPSPlayer.h"
#include <GameFramework/CharacterMovementComponent.h>

UTPSPlayerMoveComponent::UTPSPlayerMoveComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UTPSPlayerMoveComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerMoveComp->MaxWalkSpeed = WalkSpeed;
}

void UTPSPlayerMoveComponent::SetupPlayerInput(UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &UTPSPlayerMoveComponent::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &UTPSPlayerMoveComponent::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &UTPSPlayerMoveComponent::LookUp);
	PlayerInputComponent->BindAxis(TEXT("TurnRight"), this, &UTPSPlayerMoveComponent::TurnRight);
	
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &UTPSPlayerMoveComponent::Jump);
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &UTPSPlayerMoveComponent::Run);
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Released , this, &UTPSPlayerMoveComponent::Run);
}

void UTPSPlayerMoveComponent::LookUp(float Value)
{
	Player->AddControllerPitchInput(Value);
}

void UTPSPlayerMoveComponent::TurnRight(float Value)
{
	Player->AddControllerYawInput(Value);
}

void UTPSPlayerMoveComponent::MoveForward(float Value)
{
	Player->AddMovementInput(Player->GetActorForwardVector(), Value);
}

void UTPSPlayerMoveComponent::MoveRight(float Value)
{
	Player->AddMovementInput(Player->GetActorRightVector(), Value);
}

void UTPSPlayerMoveComponent::Jump()
{
	Player->Jump();
}

void UTPSPlayerMoveComponent::Run()
{
	if (PlayerMoveComp == nullptr) return;

	if(PlayerMoveComp->MaxWalkSpeed > WalkSpeed) PlayerMoveComp->MaxWalkSpeed = WalkSpeed;
	else PlayerMoveComp->MaxWalkSpeed = RunSpeed;
}