// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyFSM.generated.h"

UENUM(BlueprintType) // 상태관리를 위한 enum
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
	
protected:
	virtual void BeginPlay() override;
	
private:
	// Target에 대한 Ptr
	UPROPERTY(Transient)
	TWeakObjectPtr<class ATPSPlayer> Target;
	
	// 현재 상태를 저장
	UPROPERTY(BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	EEnemyState EnemyState;

	// Animation 변경을 위한 AnimMontage
	UPROPERTY(EditDefaultsOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> EnemyMontage;
	
	// 컴포넌트의 Owner를 형변환하여 저장해둠
	UPROPERTY()
	TObjectPtr<class AEnemy> Owner;
	
	// 컴포넌트의 Owner의 Controller
	UPROPERTY()
	TObjectPtr<class AAIController> AI;
	
	// AnimPlay 후 바꿀 State
	EEnemyState DestState = EEnemyState::Move;

	// 죽는 시간 체크
	float DieTime = 0.f;

	// Trace 간격 시간 체크
	float TraceInterval = 0.f;

	// Enemy Skill의 크기
	float SkillRadius = 0.f;
	
	// 걷기 속도
	UPROPERTY(EditDefaultsOnly, Category = Setting)
	float WalkSpeed = 500.f;

	// 근거리 공격 가능 범위
	UPROPERTY(EditDefaultsOnly, Category = Setting)
	float AttackRange = 200.f;
	
	// 원거리 공격 가능 범위
	UPROPERTY(EditDefaultsOnly, Category = Setting)
	float LDAttackRange = 3000.f;

	// 각 State 관련 처리 함수들
	void MoveState();
	void AttackState();
	void LDAttackState();
	void DieState(float DeltaTime);

	// 죽음 판정시 할 행동
	UFUNCTION()
	void EnemyDie();
	
	// Animation Play를 위한 함수
	void PlayAnim(const FName& AnimName, EEnemyState DestState);
	
	// 원거리 공격이 가능한 위치인지,
	// Target이 가만히 있다면 공격했을때 적중시킬 수 있는 위치인지
	// Trace를 통해 체크
	bool IsCanAttackPlayer() const;
	
public:	
	UEnemyFSM();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Animation Play 끝난 후 처리, AnimBP에서 호출
	UFUNCTION(BlueprintCallable, Category = Event)
	void OnEndPlayAnim();

	FORCEINLINE class ATPSPlayer* GetTarget() const { return Target.Get(); }
	
	// 데미지 받았을때 할 행동
	void TakeDamage();
};
