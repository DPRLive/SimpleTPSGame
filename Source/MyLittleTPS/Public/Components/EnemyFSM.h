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
	LDAttack,
	Die,
	AnimPlay,
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

	UPROPERTY(BlueprintReadOnly, Category = State)
		EEnemyState EnemyState;

	UPROPERTY()
		class AEnemy* Owner;

	UPROPERTY()
		class UAnimMontage* EnemyMontage;

	UPROPERTY()
		class AAIController* AI;

	TWeakObjectPtr<class ATPSPlayer> Target;

	UPROPERTY(EditDefaultsOnly, Category = Setting)
		float WalkSpeed = 500.f;

	UPROPERTY(EditDefaultsOnly, Category = Setting)
		float AttackRange = 250.f;
	
	UPROPERTY(EditDefaultsOnly, Category = Setting)
		float Hp = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = Setting)
		float LDAttackRange = 3000.f;

	// State ฐüทร
	virtual void MoveState();
	virtual void AttackState();
	void LDAttackState();

	float DieTime = 0.f;
	void DieState(float DeltaTime);

	void OnAttackDamage(float Damage);

	void PlayAnim(const FName& AnimName, EEnemyState DestState);

	UFUNCTION(BlueprintCallable, Category = Event)
		void OnEndPlayAnim();

private:
	bool IsCanAttackPlayer();
	EEnemyState DestState = EEnemyState::Move;
};
