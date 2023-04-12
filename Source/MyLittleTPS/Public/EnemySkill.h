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

	UPROPERTY(EditAnywhere, Category = Setting)
	float SkillRadius = 50.f;

	UPROPERTY(EditAnywhere, Category = Setting)
	float SkillSpeed = 1000.f;

	UPROPERTY(EditAnywhere, Category = Setting)
	float SkillDamage = 100.f;
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = Collision)
	TObjectPtr<class USphereComponent> Collision;

	UPROPERTY(EditDefaultsOnly, Category = Effect)
	TObjectPtr<class UParticleSystemComponent> WaterBall;

	UPROPERTY(VisibleAnywhere, Category = Movement)
	TObjectPtr<class UProjectileMovementComponent> MoveComp;

	UPROPERTY(EditDefaultsOnly, Category = Effect)
	TObjectPtr<class UParticleSystem> EnemyHitEmitter;
public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnEnemySkillHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
