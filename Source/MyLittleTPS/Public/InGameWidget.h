// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYLITTLETPS_API UInGameWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	float ElapsedTime = 0.f;
public:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<class ATPSPlayer> Owner;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<class UTPSPlayerSkillComponent> OwnerSkillComp;
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UWidgetSwitcher> FireTypeSwitch;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> SkillQImg;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> SkillQTime;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> SkillEImg;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> SkillETime;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> SkillCapsImg;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> SkillCapsTime;

	// 배열에 넣어서 편하게 관리
	TArray<TTuple<UImage*, UTextBlock*, bool>> SkillUIArray;

	// 쿨타임 포매팅 옵션
	FNumberFormattingOptions Options;
	
	// Widget의 BeginPlay
	virtual void NativeConstruct() override;

	// Widget의 Tick
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void SwapFireType(bool IsAutoFire);
	void SkillActive(const uint8 InSkillIndex);
};
