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
	// �̹��� ���� + ���� ���?
	if (Player->Hp + 300 > Player->MaxHp) Player->Hp = Player->MaxHp;
	else Player->Hp += 300;
	// �lŸ�� �ɱ�
	GetWorld()->GetTimerManager().SetTimer(CoolTimerHandle, 20.f, false);
}

void UTPSPlayerSkillComponent::ActiveEnergyBomb()
{
	GetWorld()->SpawnActor<AEnergyBombSkill>(AEnergyBombSkill::StaticClass(), Player->GetTransform());
	// �lŸ�� �ɱ�
	GetWorld()->GetTimerManager().SetTimer(CoolTimerHandle, 30.f, false);
}

void UTPSPlayerSkillComponent::ActiveEnergyShoot()
{
	GetWorld()->SpawnActor<AEnergyShootSkill>(AEnergyShootSkill::StaticClass(), Player->GetTransform());
	// �lŸ�� �ɱ�
	GetWorld()->GetTimerManager().SetTimer(CoolTimerHandle, 15.f, false);
}
