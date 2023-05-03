// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySkill.generated.h"

UCLASS()
class MYLITTLETPS_API AEnemySkill : public AActor
{
	GENERATED_BODY()
	
private:	
	UPROPERTY(EditAnywhere, Category = Setting)
	float SkillRadius = 50.f;

	UPROPERTY(EditAnywhere, Category = Setting)
	float SkillSpeed = 1000.f;

	UPROPERTY(EditAnywhere, Category = Setting)
	float SkillDamage = 200.f;

	UPROPERTY(VisibleAnywhere, Category = Collision)
	TObjectPtr<class USphereComponent> Collision;

	UPROPERTY(EditDefaultsOnly, Category = Effect)
	TObjectPtr<class UParticleSystemComponent> WaterBall;

	UPROPERTY(VisibleAnywhere, Category = Movement)
	TObjectPtr<class UProjectileMovementComponent> MoveComp;

	UPROPERTY(EditDefaultsOnly, Category = Effect)
	TObjectPtr<class UParticleSystem> EnemyHitEmitter;

	UFUNCTION()
	void OnEnemySkillHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
protected:
	virtual void BeginPlay() override;
	
public:
	AEnemySkill();
	virtual void Tick(float DeltaTime) override;
};
