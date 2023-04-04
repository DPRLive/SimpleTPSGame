// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySkill.generated.h"

UCLASS()
class MYLITTLETPS_API AEnemySkill : public AActor
{
	GENERATED_BODY()
	
public:	
	AEnemySkill();

	UPROPERTY(EditAnywhere, Category = "Setting")
	float SkillRadius = 50.f;
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Collision | Setting")
	TObjectPtr<class USphereComponent> Collision;

	UPROPERTY(VisibleAnywhere, Category = "Particle | Setting")
	TObjectPtr<class UParticleSystemComponent> WaterBall;

	UPROPERTY(EditAnywhere, Category = "Movement | Setting")
	TObjectPtr<class UProjectileMovementComponent> MoveComp;
public:	
	virtual void Tick(float DeltaTime) override;

};
