// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPSPlayerBaseComponent.h"
#include "TPSPlayerMoveComponent.generated.h"


UCLASS()
class MYLITTLETPS_API UTPSPlayerMoveComponent : public UTPSPlayerBaseComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Speed, meta = (AllowPrivateAccess = true))
	float WalkSpeed = 300.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Speed, meta = (AllowPrivateAccess = true))
	float RunSpeed = 600.f;

	// 카메라 이동
	void LookUp(float Value);
	void TurnRight(float Value);

	// 이동
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Jump();
	void Run();
public:
	UTPSPlayerMoveComponent();

protected:
	virtual void BeginPlay() override;

	virtual void SetupPlayerInput(class UInputComponent* PlayerInputComponent) override;
};
