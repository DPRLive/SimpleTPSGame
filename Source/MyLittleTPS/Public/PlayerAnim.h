// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnim.generated.h"

/**
 * 
 */
UCLASS()
class MYLITTLETPS_API UPlayerAnim : public UAnimInstance
{
	GENERATED_BODY()
public:
	UPlayerAnim();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly, Category = Speed)
		float RSpeed = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = Speed)
		float FSpeed = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = Jump)
		bool IsInAir = false;

	UPROPERTY(BlueprintReadOnly, Category = Rotate)
		float UpDownRotate = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = Jump)
		int32 JumpCount = 0;

	UPROPERTY(EditDefaultsOnly, Category = Montage)
		class UAnimMontage* UpperMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montage)
		class UAnimMontage* FullMontage;

	UFUNCTION(BlueprintCallable, Category = Event)
		void OnEndReload();
};
