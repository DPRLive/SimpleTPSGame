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

	UFUNCTION()
		void OnBulletBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
