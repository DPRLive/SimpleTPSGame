// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSPlayer.generated.h"

// ��ǲ ���ε��� ���� ��������Ʈ ����
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

	// ī�޶�
	UPROPERTY(VisibleAnywhere, Category = Camera)
		class USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, Category = Camera)
		class UCameraComponent* CameraComp;

	// ��
	UPROPERTY(VisibleAnywhere, Category = Gun)
		USkeletalMeshComponent* GunMesh;

	// ����Ʈ
	UPROPERTY(VisibleAnywhere, Category = Gun)
		class USpotLightComponent* GunLight;

	UPROPERTY(EditDefaultsOnly, Category = Gun)
		class USoundWave* LightOnSound;

	UPROPERTY(EditDefaultsOnly, Category = Gun)
		class USoundWave* LightOffSound;

	bool bLightOn = false;

	UFUNCTION()
		void LightToggle();

	// �̵���� ������Ʈ
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = PlayerComponent)
		class UTPSPlayerBaseComponent* MoveComp;

	// �Ѿ� �߻� ��� ������Ʈ
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = PlayerComponent)
		class UTPSPlayerBaseComponent* FireComp;

	// ��ų ��� ������Ʈ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PlayerComponent)
		class UTPSPlayerBaseComponent* SkillComp;
};
