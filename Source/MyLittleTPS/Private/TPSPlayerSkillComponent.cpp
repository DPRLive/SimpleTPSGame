// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayerSkillComponent.h"
#include "TPSPlayer.h"
#include "EnergyBombSkill.h"
#include "EnergyShootSkill.h"
#include <Kismet/GameplayStatics.h>

void UTPSPlayerSkillComponent::SetupPlayerInput(UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAction(TEXT("ActiveSkill"), IE_Pressed, this, &UTPSPlayerSkillComponent::Active);
}

float UTPSPlayerSkillComponent::GetCoolTime()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(CoolTimerHandle))
	{
		return GetWorld()->GetTimerManager().GetTimerRemaining(CoolTimerHandle);
	}
	return 0.0f;
}

void UTPSPlayerSkillComponent::Active()
{
	if (GetCoolTime() > 0.f) return;
	switch (SelectedSkill)
	{
	case 0:
		ActiveHeal();
		break;
	case 1:
		ActiveEnergyBomb();
		break;
	case 2:
		ActiveEnergyShoot();
		break;
	}
}

void UTPSPlayerSkillComponent::ActiveHeal()
{
	// 이미터 스폰 + 사운드 재생?
	if (Player->Hp + 300 > Player->MaxHp) Player->Hp = Player->MaxHp;
	else Player->Hp += 300;
	// 쿹타임 걸기
	GetWorld()->GetTimerManager().SetTimer(CoolTimerHandle, 20.f, false);
}

void UTPSPlayerSkillComponent::ActiveEnergyBomb()
{
	GetWorld()->SpawnActor<AEnergyBombSkill>(AEnergyBombSkill::StaticClass(), Player->GetTransform());
	// 쿹타임 걸기
	GetWorld()->GetTimerManager().SetTimer(CoolTimerHandle, 30.f, false);
}

void UTPSPlayerSkillComponent::ActiveEnergyShoot()
{
	GetWorld()->SpawnActor<AEnergyShootSkill>(AEnergyShootSkill::StaticClass(), Player->GetTransform());
	// 쿹타임 걸기
	GetWorld()->GetTimerManager().SetTimer(CoolTimerHandle, 15.f, false);
}
