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
private:
	
	UPROPERTY(BlueprintReadOnly, Category = Speed, meta = (AllowPrivateAccess = true))
	float RSpeed = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = Speed, meta = (AllowPrivateAccess = true))
	float FSpeed = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = Jump, meta = (AllowPrivateAccess = true))
	bool IsInAir = false;

	UPROPERTY(BlueprintReadOnly, Category = Rotate, meta = (AllowPrivateAccess = true))
	float UpDownRotate = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = Jump, meta = (AllowPrivateAccess = true))
	int32 JumpCount = 0;

	UPROPERTY(EditDefaultsOnly, Category = Montage)
	TObjectPtr<class UAnimMontage> UpperMontage;

	UFUNCTION(BlueprintCallable, Category = Event)
	void OnEndReload(bool Interruption);
	
public:
	UPlayerAnim();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	FORCEINLINE UAnimMontage* GetUpperMontage() const { return UpperMontage; }
};
