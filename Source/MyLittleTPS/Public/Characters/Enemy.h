// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "vector"
#include "Enemy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEnemyDieDelegate);

UCLASS()
class MYLITTLETPS_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemy();

protected:
	virtual void BeginPlay() override;
	
public:
	// 죽었을때 처리를 위한 델리게이트
	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = Event)
	FEnemyDieDelegate OnEnemyDie; 

	// 데미지 받았을 때 처리
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	// FSM Getter
	UFUNCTION(BlueprintCallable, Category = FSM)
	FORCEINLINE class UEnemyFSM* GetEnemyFSM() const { return FSM; };
	
	// 근거리 공격 시 사용할 무기 범위 체크 ON OFF, Anim BP에서 호출
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void AttackAreaOn();

	// 근거리 공격 시 사용할 무기 범위 체크 ON OFF, Anim BP에서 호출
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void AttackAreaOff();

	// 스킬 사용 , Anim BP에서 호출
	UFUNCTION(BlueprintCallable, Category = Skill)
	void ActivateEnemySkill();

private:
	// 적 이동 및 행동을 위한 상태머신
	UPROPERTY()
	TObjectPtr<class UEnemyFSM> FSM;
	
	// 적 근거리 공격 데미지
	UPROPERTY(EditDefaultsOnly, Category = Setting)
	float EnemyAttackDamage = 300.f;

	// 적 최대체력
	UPROPERTY(EditDefaultsOnly, Category = Setting)
	float MaxHp = 1000.f;

	// 현재 체력
	float Hp = 0.f;

	// 적 스킬, 투사체 형식으로 TSubClassOf를 BP에서 할당받아 Spawn하여 사용 
	UPROPERTY(EditAnywhere, Category = Skill)
	TSubclassOf<class AEnemySkill> EnemySkill;

	// 근거리 공격 (무기) 범위 체크
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	TObjectPtr<class UBoxComponent> AttackAreaL;

	// 근거리 공격 (무기) 범위 체크
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	TObjectPtr<class UBoxComponent> AttackAreaR;

	// 근거리 공격 체크
	UFUNCTION()
	void OnOverlapAttackArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
};
