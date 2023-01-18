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

	// ī�޶�
	UPROPERTY(VisibleAnywhere, Category = Camera)
		class USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, Category = Camera)
		class UCameraComponent* CameraComp;

	// ��
	UPROPERTY(VisibleAnywhere, Category = Gun)
		USkeletalMeshComponent* GunMesh;

	// �̵���� ������Ʈ
	UPROPERTY(VisibleAnywhere, Category = PlayerComponent)
		class UTPSPlayerBaseComponent* MoveComp;

	// �Ѿ� �߻� ��� ������Ʈ
	UPROPERTY(VisibleAnywhere, Category = PlayerComponent)
		class UTPSPlayerFireComponent* FireComp;
};
