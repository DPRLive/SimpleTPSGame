// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPSPlayerBaseComponent.h"
#include "TPSPlayerFireComponent.generated.h"

// 총기 상태에 관한 enum
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

private:
	// 총 상태 관리
	EGunState GunState;

	// 생성할 총알
	UPROPERTY(EditDefaultsOnly, Category = Bullet)
	TSubclassOf<class ABullet> BulletFactory;

	// 현재 탄창
	UPROPERTY(BlueprintReadOnly, Category = Bullet, meta=(AllowPrivateAccess = true))
	uint8 Mag;

	// 효과들
	UPROPERTY(EditDefaultsOnly, Category = Effect)
	TObjectPtr<class UParticleSystem> GunShootEmitter;

	UPROPERTY(EditDefaultsOnly, Category = Effect)
	TObjectPtr<class USoundWave> FireSound;

	UPROPERTY(EditDefaultsOnly, Category = Effect)
	TObjectPtr<class USoundWave> DryGunSound;

	UPROPERTY(EditDefaultsOnly, Category = Effect)
	TObjectPtr<class USoundWave> SwapAutoFireSound;
	
	UPROPERTY()
	TObjectPtr<class UPlayerAnim> Anim;

	// 연사 속도
	UPROPERTY(EditDefaultsOnly, Category = Settings)
	float FirePerSeconds = 9.25f;

	float CurrentTime = 0;
	
	// 탄창 관리
	UPROPERTY(EditDefaultsOnly, Category = Settings)
	uint8 MaxMag = 30;

	// 줌인 / 줌아웃
	bool IsZoom = false;

	// 최대 정조준 가능 거리
	UPROPERTY(EditDefaultsOnly)
	float MaxFineSight = 20000.f;
	
	// 연사 여부
	UPROPERTY(BlueprintReadOnly, Category = Bullet, meta=(AllowPrivateAccess = true))
	bool IsAutoFire = false;
	
	// 반동을 위한 Curve
	UPROPERTY(EditDefaultsOnly, Category = Recoil)
	TObjectPtr<class UCurveVector> RecoilCurve;
	
	float RecoilTime = 0.f;
	uint8 RecoilCount = 0;
	FVector2D RecoilValue = FVector2D(ForceInit);

	void FireState();
	void ReloadState();
	void InputFire();
	void StopFire();
	void Fire();
	void AddRecoil();
	void SwapAutoFire();
	void Zoom();
public:
	UTPSPlayerFireComponent();

	virtual void SetupPlayerInput(class UInputComponent* PlayerInputComponent) override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginPlay() override;

	DECLARE_DELEGATE_OneParam(FDelegateSwapFireType, bool)
	FDelegateSwapFireType DelegateSwapFireType;
	
	void EndReload(const bool Interruption);
};
