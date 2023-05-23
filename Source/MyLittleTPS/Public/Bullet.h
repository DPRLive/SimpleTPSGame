// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

UCLASS()
class MYLITTLETPS_API ABullet : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleAnywhere, Category = Collision)
	TObjectPtr<class UBoxComponent> BoxComp;

	UPROPERTY(VisibleAnywhere, Category = Mesh)
	TObjectPtr<class UStaticMeshComponent> MeshComp;
	
	// 총알 이동
	UPROPERTY(VisibleAnywhere, Category = Component)
	TObjectPtr<class UProjectileMovementComponent> MoveComp;

	UPROPERTY(EditDefaultsOnly, Category = Settings)
	float BulletInitSpeed = 40000.f;

	UPROPERTY(EditDefaultsOnly, Category = Settings)
	float BulletMaxSpeed = 40000.f;

	UPROPERTY(EditDefaultsOnly, Category = Effect)
	TObjectPtr<class UParticleSystem> HitEmitter;

	UPROPERTY(EditDefaultsOnly, Category = Effect)
	TObjectPtr<class UParticleSystem> EnemyHitEmitter;

	UPROPERTY(EditDefaultsOnly, Category = Effect)
	TObjectPtr<class UMaterial> BulletHole;

	UFUNCTION()
	void OnBulletHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
public:	
	ABullet();

protected:
	virtual void BeginPlay() override;
};
