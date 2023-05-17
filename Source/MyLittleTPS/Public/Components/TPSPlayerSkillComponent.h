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
	// TODO : 쿨타임 걸기, 스킬 컴포넌트로 관리할까 ? ? ?
	// TODO : 귀찮은데 그냥 떄려 박을까 .? ? ?
	// TODO : 장전시간 줄이기.
	UTPSPlayerSkillComponent();
	virtual void SetupPlayerInput(class UInputComponent* PlayerInputComponent) override;
	// 힐량 설정
	UPROPERTY(EditDefaultsOnly, Category = Skill)
	float HealthWeight = 300.f;
	
	UPROPERTY(EditDefaultsOnly, Category = Effect)
	TObjectPtr<class UParticleSystem> HealEmitter;

	UPROPERTY(EditDefaultsOnly, Category = Effect)
	TObjectPtr<class USoundWave> HealSound;

	FTimerHandle CoolTimerHandle;

	UFUNCTION(BlueprintCallable)
	float GetCoolTime();

	void Active();
	void ActiveHeal();
	void ActiveEnergyBomb();
	void ActiveEnergyShoot();
};
