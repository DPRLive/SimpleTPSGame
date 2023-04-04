// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/EnemyFSM.h"
#include "LDEnemyFSM.generated.h"

/**
 * 
 */
UCLASS()
class MYLITTLETPS_API ULDEnemyFSM : public UEnemyFSM
{
	GENERATED_BODY()

private:
	bool IsCanAttackPlayer();

protected:
	virtual void BeginPlay() override;

public:
	//UPROPERTY(EditDefaultsOnly, Category = Setting)
		//float LDAttackRange = 3000.f;

	virtual void MoveState() override;
	virtual void AttackState() override;

};
