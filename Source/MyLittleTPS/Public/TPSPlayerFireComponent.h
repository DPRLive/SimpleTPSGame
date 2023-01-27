// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPSPlayerBaseComponent.h"
#include "TPSPlayerFireComponent.generated.h"

/**
 * 
 */
UENUM(BlueprintType) 
enum class EGunState : uint8
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
	// ÃÑ »óÅÂ °ü¸®
	EGunState GunState;

	void FireState();
	void ReloadState();

	UPROPERTY(EditDefaultsOnly, Category = Settings)
		TSubclassOf<class ABullet> BulletFactory;
	
	UPROPERTY()
		class UPlayerAnim* Anim;

	// µô·¹ÀÌ Ã¼Å©
	UPROPERTY(EditDefaultsOnly, Category = Settings)
		float FirePerSeconds = 9.25f;
	float CurrentTime = 0;

	// ÃÑ ¹ß»ç
	bool IsAutoFire = false;
	// FTimerHandle FireTimer;
	void InputFire();
	void StopFire();
	void Fire();
	void SwapAutoFire();

	// ÅºÃ¢ °ü¸®
	UPROPERTY(EditDefaultsOnly, Category = Settings)
		uint8 MaxMag = 30;

	uint8 Mag;

	UFUNCTION(BlueprintCallable, Category = Reload)
		void EndReload();
	// ÁÜÀÎ / ÁÜ¾Æ¿ô
	bool IsZoom = false;
	void Zoom();
};
