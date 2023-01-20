// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnergyShootSkill.generated.h"

UCLASS()
class MYLITTLETPS_API AEnergyShootSkill : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnergyShootSkill();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(VisibleAnywhere, Category = Collision)
		class UBoxComponent* SkillRangeCollision;

	UPROPERTY(VisibleAnywhere, Category = Movement)
		class UProjectileMovementComponent* MoveComp;

	// 이미터 추가하면 좋겠당^ㅡ^
	UFUNCTION()
		void OnBeginOverlapEnemy(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
