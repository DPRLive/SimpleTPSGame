// Copyright Epic Games, Inc. All Rights Reserved.


#include "MyLittleTPSGameModeBase.h"
#include "MonsterSpawner.h"
#include "Characters/TPSPlayer.h"

#include <Kismet/GameplayStatics.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(MyLittleTPSGameModeBase)

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

	if(auto PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		if(auto Player = Cast<ATPSPlayer>(PlayerCharacter))
		{
			Player->DelegateCharacterDie.BindUFunction(this, TEXT("OnCharacterDie"));
		}
	}
}

void AMyLittleTPSGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	ElapseTime += DeltaSeconds;
	if(ElapseTime > MonsterSpawnInterval)
	{
		// 스폰되는 마릿수는 랜덤
		uint32 NumOfSpawn = FMath::RandRange(MinSpawnMonsterNumsPerInterval, MaxSpawnMonsterNumsPerInterval);
		
		for (uint32 i = 0; i < NumOfSpawn; ++i)
		{
			if(NumberOfMonsters >= NumberOfMonstersLimit) break;
			
			// 스폰 위치도 랜덤
			uint32 SpawnIndex = FMath::RandRange(0, MonsterSpawners.Num() - 1);
			MonsterSpawners[SpawnIndex]->SpawnMonster();
			NumberOfMonsters++;
		}
		
		ElapseTime = 0.f;
		// 다음 생성될 시간을 설정 된 Min~Max 사이에서 랜덤으로 결정
		MonsterSpawnInterval = FMath::RandRange(MonsterSpawnIntervalMin, MonsterSpawnIntervalMax);
	}
}

void AMyLittleTPSGameModeBase::DecreaseNumOfMonster()
{
	if(NumberOfMonsters <= 1) NumberOfMonsters = 0;
	else NumberOfMonsters--;
}