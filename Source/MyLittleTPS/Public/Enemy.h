// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "vector"
#include "Enemy.generated.h"

UCLASS()
class MYLITTLETPS_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemy();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
		class UBoxComponent* AttackAreaL;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
		class UBoxComponent* AttackAreaR;

	UFUNCTION(BlueprintCallable, Category = Weapon)
		void AttackAreaOn();

	UFUNCTION(BlueprintCallable, Category = Weapon)
		void AttackAreaOff();

	UFUNCTION()
		void OnOverlapAttackArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FSMComponent)
		class UEnemyFSM* FSM;
	
};
