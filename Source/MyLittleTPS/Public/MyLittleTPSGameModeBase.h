// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyLittleTPSGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class MYLITTLETPS_API AMyLittleTPSGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AMonsterSpawner> MonsterSpawner;

	TArray<class AMonsterSpawner*> MonsterSpawners;

	// 몬스터 개체 수 관리
	UPROPERTY(EditDefaultsOnly, Category = Setting)
	uint32 NumberOfMonstersLimit = 400;

	// 몬스터가 한번 스폰시 스폰될 최소 마릿수
	UPROPERTY(EditDefaultsOnly, Category = Setting)
	int32 MinSpawnMonsterNumsPerInterval = 5;

	// 몬스터가 한번 스폰시 스폰될 최대 마릿수
	UPROPERTY(EditDefaultsOnly, Category = Setting)
	int32 MaxSpawnMonsterNumsPerInterval = 10;

	// 몬스터 스폰주기 최소 시간
	UPROPERTY(EditDefaultsOnly, Category = Setting)
	float MonsterSpawnIntervalMin = 2.f;

	// 몬스터 스폰주기 최대 시간
	UPROPERTY(EditDefaultsOnly, Category = Setting)
	float MonsterSpawnIntervalMax = 10.f;

	float MonsterSpawnInterval;
	float ElapseTime = 0.f;
public:
	AMyLittleTPSGameModeBase();
	
	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds) override;
};
