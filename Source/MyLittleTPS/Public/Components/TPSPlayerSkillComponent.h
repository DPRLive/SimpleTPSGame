// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPSPlayerBaseComponent.h"
#include "TPSPlayerSkillComponent.generated.h"

UENUM( BlueprintType )
enum class ESkillType : uint8
{
	EnergyShoot,
	Heal,
	EnergyBomb
};

UCLASS()
class MYLITTLETPS_API UTPSPlayerSkillComponent : public UTPSPlayerBaseComponent
{
	GENERATED_BODY()

public:
	// Cool timer handle 3개 설정.. 시간 없으니 설계 없이 하나에 다 때려 박자..
	FTimerHandle CoolTimerHandleHeal, CoolTimerHandleEnergyBomb, CoolTimerHandleEnergyShoot;

	// 스킬 Active를 UI에 알리기 위한 Delegate
	DECLARE_DELEGATE_OneParam(FDelegateSkillActive, const uint8)
	FDelegateSkillActive DelegateSkillActive;
	
	UTPSPlayerSkillComponent();

	virtual void SetupPlayerInput(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly)
	float HealCoolTime = 20.f;

	UPROPERTY(EditDefaultsOnly)
	float EnergyShootCooltime = 8.f;

	UPROPERTY(EditDefaultsOnly)
	float EnergyBombCoolTime = 20.f;
	
	// 힐 설정
	UPROPERTY(EditDefaultsOnly, Category = Skill)
	float HealthWeight = 300.f;
	
	UPROPERTY(EditDefaultsOnly, Category = Effect)
	TObjectPtr<class UParticleSystem> HealEmitter;

	UPROPERTY(EditDefaultsOnly, Category = Effect)
	TObjectPtr<class USoundWave> HealSound;

	UFUNCTION(BlueprintCallable)
	float GetCoolTime(ESkillType InSkillType);

	void ActiveHeal();
	void ActiveEnergyBomb();
	void ActiveEnergyShoot();
};
