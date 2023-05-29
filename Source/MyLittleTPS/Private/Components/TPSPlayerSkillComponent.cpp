// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/TPSPlayerSkillComponent.h"
#include "Characters/TPSPlayer.h"
#include "Skills/EnergyBombSkill.h"
#include "SKills/EnergyShootSkill.h"

#include <Kismet/GameplayStatics.h>
#include <Particles/ParticleSystem.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(TPSPlayerSkillComponent)

UTPSPlayerSkillComponent::UTPSPlayerSkillComponent()
{
	ConstructorHelpers::FObjectFinder<UParticleSystem> HealEmitterTemp(TEXT("/Script/Engine.ParticleSystem'/Game/Effects/P_Heal.P_Heal'"));
	if (HealEmitterTemp.Succeeded()) HealEmitter = HealEmitterTemp.Object;

	ConstructorHelpers::FObjectFinder<USoundWave> HealSoundTemp(TEXT("/Script/Engine.SoundWave'/Game/Effects/Sounds/Healing.Healing'"));
	if (HealSoundTemp.Succeeded()) HealSound = HealSoundTemp.Object;
}

void UTPSPlayerSkillComponent::SetupPlayerInput(UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAction(TEXT("ActiveSkill1"), IE_Pressed, this, &UTPSPlayerSkillComponent::ActiveEnergyShoot);
	PlayerInputComponent->BindAction(TEXT("ActiveSkill2"), IE_Pressed, this, &UTPSPlayerSkillComponent::ActiveHeal);
	PlayerInputComponent->BindAction(TEXT("ActiveSkill3"), IE_Pressed, this, &UTPSPlayerSkillComponent::ActiveEnergyBomb);
}

float UTPSPlayerSkillComponent::GetCoolTime(ESkillType InSkillType)
{
	FTimerHandle* CoolTimerHandle = nullptr;

	switch (InSkillType)
	{
	case ESkillType::EnergyShoot:
		CoolTimerHandle = &CoolTimerHandleEnergyShoot;
		break;
	case ESkillType::Heal:
		CoolTimerHandle = &CoolTimerHandleHeal;
		break;
	case ESkillType::EnergyBomb:
		CoolTimerHandle = &CoolTimerHandleEnergyBomb;
		break;
	}
	
	if (CoolTimerHandle != nullptr && GetWorld()->GetTimerManager().IsTimerActive(*CoolTimerHandle))
	{
		return GetWorld()->GetTimerManager().GetTimerRemaining(*CoolTimerHandle);
	}
	return 0.0f;
} 

void UTPSPlayerSkillComponent::ActiveHeal()
{
	if(GetCoolTime(ESkillType::Heal) > 0.f) { return; }
	
	// 이미터 스폰 + 사운드 재생?
	if (HealEmitter != nullptr) UGameplayStatics::SpawnEmitterAttached(HealEmitter, Player->GetMesh(), NAME_None, FVector(ForceInit), FRotator(ForceInit), FVector(0.5f, 0.5f, 1.f));
	if (HealSound != nullptr) UGameplayStatics::PlaySoundAtLocation(GetWorld(), HealSound, Player->GetActorLocation(), 1.f, 1.f, 0.2f);

	Player->AddHealth(HealthWeight);
	// 쿹타임 걸기
	GetWorld()->GetTimerManager().SetTimer(CoolTimerHandleHeal, HealCoolTime, false);
	DelegateSkillActive.ExecuteIfBound(static_cast<uint8>(ESkillType::Heal));
}

void UTPSPlayerSkillComponent::ActiveEnergyBomb()
{
	if(GetCoolTime(ESkillType::EnergyBomb) > 0.f) { return; }
	
	GetWorld()->SpawnActor<AEnergyBombSkill>(AEnergyBombSkill::StaticClass(), Player->GetTransform());
	// 쿹타임 걸기
	GetWorld()->GetTimerManager().SetTimer(CoolTimerHandleEnergyBomb, EnergyBombCoolTime, false);
	DelegateSkillActive.ExecuteIfBound(static_cast<uint8>(ESkillType::EnergyBomb));
}

void UTPSPlayerSkillComponent::ActiveEnergyShoot()
{
	if(GetCoolTime(ESkillType::EnergyShoot) > 0.f) { return; }

	GetWorld()->SpawnActor<AEnergyShootSkill>(AEnergyShootSkill::StaticClass(), Player->GetTransform());
	// 쿹타임 걸기
	GetWorld()->GetTimerManager().SetTimer(CoolTimerHandleEnergyShoot, EnergyShootCooltime, false);
	DelegateSkillActive.ExecuteIfBound(static_cast<uint8>(ESkillType::EnergyShoot));
}
