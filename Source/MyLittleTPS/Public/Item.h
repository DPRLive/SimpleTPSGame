// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class MYLITTLETPS_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AItem();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
		class USceneComponent* Center;

	UPROPERTY(VisibleAnywhere, Category = Components)
		class URotatingMovementComponent* RotatingComponent;

	// 공은 5개 까지만 추가
	UPROPERTY(EditDefaultsOnly, Category = Settings)
		uint8 MaxBall = 5;

	uint8 NowBallCount = 0;

	UPROPERTY(EditDefaultsOnly, Category = Settings)
		float RangeRadius = 200.f;

	UPROPERTY(EditDefaultsOnly, Category = Settings)
		float BallDamage = 400.f;

	FTimerHandle DestroyTimer; 
	void AddBall();
	void CreateNewBall(FVector& Location);

	UFUNCTION()
		void OnAttackDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Effects
	UPROPERTY(EditDefaultsOnly, Category = Effect)
		class UParticleSystem* HitEmitter;

	UPROPERTY(EditDefaultsOnly, Category = Effect)
		class USoundWave* ItemSound;

	UPROPERTY(EditDefaultsOnly, Category = Effect)
		class USoundWave* HitSound;
};
