// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameWidget.h"
#include "Characters/TPSPlayer.h"
#include "Components/TPSPlayerFireComponent.h"
#include "Components/TPSPlayerSkillComponent.h"

#include <Components/WidgetSwitcher.h>
#include <Components/Image.h>
#include <Components/TextBlock.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(InGameWidget)

void UInGameWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if(Owner != nullptr)
	{
		if(auto FireComp = Owner->GetFireComp())
		{
			FireComp->DelegateSwapFireType.BindUObject(this, &UInGameWidget::SwapFireType);
		}
		if(auto SkillComp = Owner->GetSkillComp())
		{
			OwnerSkillComp = SkillComp;
			OwnerSkillComp->DelegateSkillActive.BindUObject(this, &UInGameWidget::SkillActive);
		}
	}

	SkillUIArray.Add({SkillQImg, SkillQTime, false});
	SkillUIArray.Add({SkillEImg, SkillETime, false});
	SkillUIArray.Add({SkillCapsImg, SkillCapsTime, false});

	Options.MaximumFractionalDigits = 1;
	Options.MinimumFractionalDigits = 1;
}

void UInGameWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	ElapsedTime += InDeltaTime;

	if(ElapsedTime >= 0.05f)
	{
		// 뭔가 매 틱 갱신하기는 아까우니 0.05초마다 갱신
		for(uint8 i = 0; i < SkillUIArray.Num(); i++)
		{
			if(SkillUIArray[i].Get<2>())
			{
				float coolTime = OwnerSkillComp->GetCoolTime(static_cast<ESkillType>(i));
			
				if(FMath::IsNearlyZero(coolTime))
				{
					SkillUIArray[i].Get<0>()->SetOpacity(1.f);
					SkillUIArray[i].Get<1>()->SetOpacity(0.f);
					SkillUIArray[i].Get<2>() = false;
				}
				else
				{
					SkillUIArray[i].Get<1>()->SetText(FText::AsNumber(coolTime, &Options));
				}
			}
		}
		ElapsedTime = 0.f;
	}
}

void UInGameWidget::SwapFireType(bool IsAutoFire)
{
	// index 0에 단발, 1에 연발 넣어둠
	FireTypeSwitch->SetActiveWidgetIndex(IsAutoFire);
}

void UInGameWidget::SkillActive(const uint8 InSkillIndex)
{
	SkillUIArray[InSkillIndex].Get<0>()->SetOpacity(0.3f);
	SkillUIArray[InSkillIndex].Get<1>()->SetOpacity(1.f);
	SkillUIArray[InSkillIndex].Get<2>() = true;
}
