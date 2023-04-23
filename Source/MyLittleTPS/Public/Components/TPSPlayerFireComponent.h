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
	// 총 상태 관리
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

	// 딜레이 체크
	UPROPERTY(EditDefaultsOnly, Category = Settings)
		float FirePerSeconds = 9.25f;
	float CurrentTime = 0;

	// 총 발사
	bool IsAutoFire = false;
	void InputFire();
	void StopFire();
	void Fire();
	void SwapAutoFire();

	// 탄창 관리
	UPROPERTY(EditDefaultsOnly, Category = Settings)
		uint8 MaxMag = 30;

	void EndReload(bool Interruption);
	// 줌인 / 줌아웃
	bool IsZoom = false;
	void Zoom();

	// 반동
	UPROPERTY(EditDefaultsOnly, Category = Recoil)
		class UCurveVector* RecoilCurve;

	uint8 RecoilCount = 0;
	FVector2D RecoilValue = FVector2D(ForceInit);

	void AddRecoil();
};
