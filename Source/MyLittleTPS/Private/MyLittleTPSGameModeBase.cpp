﻿// Copyright Epic Games, Inc. All Rights Reserved.


#include "MyLittleTPSGameModeBase.h"
#include "MonsterSpawner.h"

#include <Kismet/GameplayStatics.h>

AMyLittleTPSGameModeBase::AMyLittleTPSGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
	MonsterSpawnInterval = MonsterSpawnIntervalMin;
}

void AMyLittleTPSGameModeBase::StartPlay()
{
	Super::StartPlay();

	// 월드에 배치된 스포너를 등록한다.
	TArray<AActor*> OutArrays;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), MonsterSpawner, OutArrays);
	
	for(const auto& Spawner : OutArrays)
	{
		if(AMonsterSpawner* SpawnerCast = Cast<AMonsterSpawner>(Spawner))
		{
			MonsterSpawners.Add(SpawnerCast);
		}
	}
}

void AMyLittleTPSGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	// TODO: 몬스터 스킬 부시기 가능으로 만들기
	ElapseTime += DeltaSeconds;
	if(ElapseTime > MonsterSpawnInterval)
	{
		// 스폰되는 마릿수는 랜덤 (최소 5 ~ 최대 20)
		uint32 NumOfSpawn = FMath::RandRange(MinSpawnMonsterNumsPerInterval, MaxSpawnMonsterNumsPerInterval);
		
		for (uint32 i = 0; i < NumOfSpawn; ++i)
		{
			// 스폰 위치도 랜덤
			uint32 SpawnIndex = FMath::RandRange(0, MonsterSpawners.Num() - 1);
			MonsterSpawners[SpawnIndex]->SpawnMonster();
		}
		
		ElapseTime = 0.f;
		// 다음 생성될 시간을 설정 된 Min~Max 사이에서 랜덤으로 결정
		MonsterSpawnInterval = FMath::RandRange(MonsterSpawnIntervalMin, MonsterSpawnIntervalMax);
	}
}
