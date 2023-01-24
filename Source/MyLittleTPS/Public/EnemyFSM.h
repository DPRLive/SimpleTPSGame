// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyFSM.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Move,
	Attack,
	Damaged,
	Die,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYLITTLETPS_API UEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEnemyFSM();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	EEnemyState EnemyState;

	UPROPERTY()
		class AEnemy* Owner;

	UPROPERTY()
		class AAIController* AI;

	TWeakObjectPtr<class ATPSPlayer> Target;

	UPROPERTY(EditDefaultsOnly, Category = Setting)
		float WalkSpeed = 500.f;

	UPROPERTY(EditDefaultsOnly, Category = Setting)
		float AttackRange = 200.f;
	
	UPROPERTY(EditDefaultsOnly, Category = Setting)
		float Hp = 1000.f;

	// State ฐüทร
	float CurrentTime = 0;

	UPROPERTY(EditDefaultsOnly, Category = Setting)
		float AttackDelay = 2.f;

	void MoveState();
	void AttackState();
	void DamageState();
	void DieState(float DeltaTime);

	void Attack();
	void OnAttackDamage(float Damage);
};
