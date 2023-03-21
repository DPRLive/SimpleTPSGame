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

	UPROPERTY(EditDefaultsOnly, Category = Bullet)
		TSubclassOf<class ABullet> BulletFactory;
	
	UPROPERTY(BlueprintReadOnly, Category = Bullet)
		uint8 Mag;
	
	UPROPERTY(EditDefaultsOnly, Category = Bullet)
		class UParticleSystem* GunShootEmitter;

	UPROPERTY(EditDefaultsOnly, Category = Bullet)
		class USoundWave* FireSound;

	UPROPERTY(EditDefaultsOnly, Category = Bullet)
		class USoundWave* DryGunSound;

	UPROPERTY()
		class UPlayerAnim* Anim;

	// µô·¹ÀÌ Ã¼Å©
	UPROPERTY(EditDefaultsOnly, Category = Settings)
		float FirePerSeconds = 9.25f;
	float CurrentTime = 0;

	// ÃÑ ¹ß»ç
	bool IsAutoFire = false;
	void InputFire();
	void StopFire();
	void Fire();
	void SwapAutoFire();

	// ÅºÃ¢ °ü¸®
	UPROPERTY(EditDefaultsOnly, Category = Settings)
		uint8 MaxMag = 30;

	void EndReload(bool Interruption);
	// ÁÜÀÎ / ÁÜ¾Æ¿ô
	bool IsZoom = false;
	void Zoom();

	// ¹Ýµ¿
	UPROPERTY(EditDefaultsOnly, Category = Recoil)
		class UCurveVector* RecoilCurve;

	uint8 RecoilCount = 0;
	FVector2D RecoilValue = FVector2D(ForceInit);

	void AddRecoil();
};
