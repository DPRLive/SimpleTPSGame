// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSPlayer.generated.h"

// 인풋 바인딩을 위한 델리게이트 선언
DECLARE_MULTICAST_DELEGATE_OneParam(FInputBindingDelegate, class UInputComponent*)

UCLASS()
class MYLITTLETPS_API ATPSPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	ATPSPlayer();
	
protected:
	virtual void BeginPlay() override;

public:	
	FInputBindingDelegate InputBindingDelegate;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// 데미지 받았을 때 처리
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	// 최대체력
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Settings, meta = (AllowPrivateAccess = "true"))
	float MaxHp = 1000;

	// 현재 체력
	UPROPERTY(BlueprintReadOnly, Category = Settings,  meta = (AllowPrivateAccess = "true"))
	float Hp = 0.f;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UTPSPlayerBaseComponent* GetFireComp() const { return FireComp; }	
	FORCEINLINE class UCameraComponent* GetCameraComp() const { return CameraComp; }	
	FORCEINLINE USkeletalMeshComponent* GetGunMesh() const { return GunMesh; }	
private:
	// 카메라 관련
	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<class USpringArmComponent> SpringArmComp;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<class UCameraComponent> CameraComp;

	// 총
	UPROPERTY(VisibleAnywhere, Category = Gun)
	TObjectPtr<USkeletalMeshComponent> GunMesh;

	// 라이트
	UPROPERTY(VisibleAnywhere, Category = Gun)
	TObjectPtr<class USpotLightComponent> GunLight;

	// 라이트 키는 소리
	UPROPERTY(EditDefaultsOnly, Category = Gun)
	TObjectPtr<class USoundWave> LightOnSound;

	// 라이트 끄는 소리
	UPROPERTY(EditDefaultsOnly, Category = Gun)
	TObjectPtr<class USoundWave> LightOffSound;

	bool bLightOn = false;

	UFUNCTION()
	void LightToggle();

	// 이동담당 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PlayerComponent, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UTPSPlayerBaseComponent> MoveComp;

	// 총알 발사 담당 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = PlayerComponent)
	TObjectPtr<class UTPSPlayerBaseComponent> FireComp;

	// 스킬 담당 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = PlayerComponent)
	TObjectPtr<class UTPSPlayerBaseComponent> SkillComp;
};