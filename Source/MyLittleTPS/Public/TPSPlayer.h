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

	UPROPERTY(EditDefaultsOnly, Category = Settings)
		float MaxHp = 1000;

	UPROPERTY(VisibleAnywhere, Category = Settings)
		float Hp;

	UFUNCTION(BlueprintCallable, Category = Event)
		void OnAttackDamage(float Damage);

	// 카메라
	UPROPERTY(VisibleAnywhere, Category = Camera)
		class USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, Category = Camera)
		class UCameraComponent* CameraComp;

	// 총
	UPROPERTY(VisibleAnywhere, Category = Gun)
		USkeletalMeshComponent* GunMesh;

	// 라이트
	UPROPERTY(VisibleAnywhere, Category = Gun)
		class USpotLightComponent* GunLight;

	UPROPERTY(EditDefaultsOnly, Category = Gun)
		class USoundWave* LightOnSound;

	UPROPERTY(EditDefaultsOnly, Category = Gun)
		class USoundWave* LightOffSound;

	bool bLightOn = false;

	UFUNCTION()
		void LightToggle();

	// 이동담당 컴포넌트
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = PlayerComponent)
		class UTPSPlayerBaseComponent* MoveComp;

	// 총알 발사 담당 컴포넌트
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = PlayerComponent)
		class UTPSPlayerBaseComponent* FireComp;

	// 스킬 담당 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PlayerComponent)
		class UTPSPlayerBaseComponent* SkillComp;
};
