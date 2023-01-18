// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayerBaseComponent.h"
#include "TPSPlayer.h"
#include <GameFramework/CharacterMovementComponent.h>

UTPSPlayerBaseComponent::UTPSPlayerBaseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// initializeComponent 사용 선언
	bWantsInitializeComponent = true;
}


void UTPSPlayerBaseComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTPSPlayerBaseComponent::InitializeComponent()
{
	// Super 꼭!!!
	Super::InitializeComponent();

	// Begin Play 전에 Player를 알고 있어야 함.
	Player = Cast<ATPSPlayer>(GetOwner());
	if (Player == nullptr) return;

	PlayerMoveComp = Cast<UCharacterMovementComponent>(Player->GetMovementComponent());

	Player->InputBindingDelegate.AddUObject(this, &UTPSPlayerBaseComponent::SetupPlayerInput);
}
