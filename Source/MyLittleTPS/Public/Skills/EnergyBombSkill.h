// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnergyBombSkill.generated.h"

UCLASS()
class MYLITTLETPS_API AEnergyBombSkill : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnergyBombSkill();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, Category = Collision)
	TObjectPtr<class USphereComponent> SkillRangeCollision;

	UPROPERTY(EditDefaultsOnly, Category = Effect)
	TObjectPtr<class UParticleSystem> SkillEmitter;

	UPROPERTY(EditDefaultsOnly, Category = Effect)
	TObjectPtr<class UParticleSystem> HitEmitter;

	UPROPERTY(EditDefaultsOnly, Category = Effect)
	TObjectPtr<class USoundWave> SkillSound;

	UPROPERTY(EditDefaultsOnly, Category = Effect)
	TObjectPtr<class USoundWave> HitSound;

	UFUNCTION()
	void OnBeginOverlapEnemy(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                         const FHitResult& SweepResult);
};
