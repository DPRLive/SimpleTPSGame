// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

UCLASS()
class MYLITTLETPS_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	ABullet();

protected:
	virtual void BeginPlay() override;

public:	

	UPROPERTY(VisibleAnywhere, Category = Collision)
		class USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, Category = Mesh)
		class UStaticMeshComponent* MeshComp;

	// ÃÑ¾Ë ÀÌµ¿
	UPROPERTY(VisibleAnywhere, Category = Component)
		class UProjectileMovementComponent* MoveComp;

	UPROPERTY(EditDefaultsOnly, Category = Settings)
		float BulletInitSpeed = 60000;

	UPROPERTY(EditDefaultsOnly, Category = Settings)
		float BulletMaxSpeed = 80000;

	UPROPERTY(EditDefaultsOnly, Category = Effect)
		class UParticleSystem* HitEmitter;

	UPROPERTY(EditDefaultsOnly, Category = Effect)
		class UParticleSystem* EnemyHitEmitter;

	UPROPERTY(EditDefaultsOnly, Category = Effect)
		class UMaterial* BulletHole;

	UFUNCTION()
		void OnBulletHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
