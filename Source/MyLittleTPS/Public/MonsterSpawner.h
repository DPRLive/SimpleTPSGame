// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonsterSpawner.generated.h"


UCLASS()
class MYLITTLETPS_API AMonsterSpawner : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UCapsuleComponent> CapsuleComp;

	// 생성할 몬스터 설정
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AEnemy> EnemyFactory;

public:	
	AMonsterSpawner();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// 몬스터 스폰 함수
	void SpawnMonster() const;
};
