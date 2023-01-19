// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPSPlayerBaseComponent.h"
#include "TPSPlayerFireComponent.generated.h"

/**
 * 
 */
UENUM(BlueprintType) 
enum class EFireState : uint8
{
	Idle,
	Fire,
	Reload
};

UCLASS()
class MYLITTLETPS_API UTPSPlayerFireComponent : public UTPSPlayerBaseComponent
{
	GENERATED_BODY()
	
public:

	UTPSPlayerFireComponent();

	virtual void SetupPlayerInput(class UInputComponent* PlayerInputComponent) override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginPlay() override;
	// �� ���� ����
	EFireState FireState = EFireState::Idle;

	UPROPERTY(EditDefaultsOnly, Category = Settings)
		TSubclassOf<class ABullet> BulletFactory;

	// ������ üũ
	UPROPERTY(EditDefaultsOnly, Category = Settings)
		float FirePerSeconds = 9.25f;
	float CurrentTime = 0;

	// �� �߻�
	bool IsAutoFire = false;
	// FTimerHandle FireTimer;
	void InputFire();
	void StopFire();
	void Fire();
	void SwapAutoFire();

	// źâ ����
	UPROPERTY(EditDefaultsOnly, Category = Settings)
		uint8 MaxMag = 30;

	uint8 Mag;
	void Reload();
};