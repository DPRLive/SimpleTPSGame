// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemDrop.generated.h"

UCLASS()
class MYLITTLETPS_API AItemDrop : public AActor
{
	GENERATED_BODY()
	
public:	
	AItemDrop();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = Collision)
		class UBoxComponent* Box;

	UPROPERTY(EditDefaultsOnly, Category = Effect)
		class UParticleSystemComponent* ItemPreview;

	UPROPERTY(EditDefaultsOnly, Category = Effect)
		class UParticleSystem* OverlapEmitter;

	UPROPERTY(EditDefaultsOnly, Category = Effect)
		class USoundWave* ItemOverlapSound;

	UFUNCTION()
		void OnPlayerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
