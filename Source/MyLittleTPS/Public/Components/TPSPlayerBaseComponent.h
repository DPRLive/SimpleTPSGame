// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TPSPlayerBaseComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYLITTLETPS_API UTPSPlayerBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTPSPlayerBaseComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
		class ATPSPlayer* Player;
	
	UPROPERTY()
		class UCharacterMovementComponent* PlayerMoveComp;

	virtual void InitializeComponent() override;

	virtual void SetupPlayerInput(class UInputComponent* PlayerInputComponent) {};
};
