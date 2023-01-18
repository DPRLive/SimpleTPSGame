// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayerBaseComponent.h"
#include "TPSPlayer.h"
#include <GameFramework/CharacterMovementComponent.h>

UTPSPlayerBaseComponent::UTPSPlayerBaseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// initializeComponent ��� ����
	bWantsInitializeComponent = true;
}


void UTPSPlayerBaseComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTPSPlayerBaseComponent::InitializeComponent()
{
	// Super ��!!!
	Super::InitializeComponent();

	// Begin Play ���� Player�� �˰� �־�� ��.
	Player = Cast<ATPSPlayer>(GetOwner());
	if (Player == nullptr) return;

	PlayerMoveComp = Cast<UCharacterMovementComponent>(Player->GetMovementComponent());

	Player->InputBindingDelegate.AddUObject(this, &UTPSPlayerBaseComponent::SetupPlayerInput);
}
