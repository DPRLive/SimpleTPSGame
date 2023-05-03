// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPSPlayerBaseComponent.h"
#include "TPSPlayerSkillComponent.generated.h"

/**
 * 스킬 3개 모두 사용으로 만들어야함
 */
UCLASS()
class MYLITTLETPS_API UTPSPlayerSkillComponent : public UTPSPlayerBaseComponent
{
	GENERATED_BODY()

public:
	UTPSPlayerSkillComponent();
	virtual void SetupPlayerInput(class UInputComponent* PlayerInputComponent) override;

	uint8 SelectedSkill = 1; // 0 회복 1 기방출 2 전방공격 예정..

	UPROPERTY(EditDefaultsOnly, Category = Bullet)
		class UParticleSystem* HealEmitter;

	UPROPERTY(EditDefaultsOnly, Category = Bullet)
		class USoundWave* HealSound;

	FTimerHandle CoolTimerHandle;

	UFUNCTION(BlueprintCallable)
		float GetCoolTime();

	void Active();
	void ActiveHeal();
	void ActiveEnergyBomb();
	void ActiveEnergyShoot();
};
