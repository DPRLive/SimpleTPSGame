// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDrop.h"
#include <Components/BoxComponent.h>
#include <Components/StaticMeshComponent.h>
#include "TPSPlayer.h"
#include "Item.h"

AItemDrop::AItemDrop()
{
	PrimaryActorTick.bCanEverTick = false;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetCollisionProfileName(TEXT("ItemDrop"));
	SetRootComponent(Box);

	SkillPreview = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SkillPreview"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> TempMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if (TempMesh.Succeeded())
	{
		SkillPreview->SetStaticMesh(TempMesh.Object);
		SkillPreview->SetRelativeScale3D(FVector(0.5f));
		SkillPreview->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	SkillPreview->SetupAttachment(RootComponent);
}

void AItemDrop::BeginPlay()
{
	Super::BeginPlay();
	
	Box->OnComponentBeginOverlap.AddDynamic(this, &AItemDrop::OnPlayerOverlap);
}

// Called every frame
void AItemDrop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItemDrop::OnPlayerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto PresetName = OverlappedComponent->GetCollisionProfileName();
	if (PresetName.Compare(FName(TEXT("Player"))))
	{
		auto Player = Cast<ATPSPlayer>(OtherActor);
		if (Player != nullptr)
		{
			TArray<AActor*> OutActors; // 붙어있는 아이템이 있는지?
			Player->GetAttachedActors(OutActors);
			if (OutActors.Num() < 1) // 안 붙어있으면 생성해서 붙여준다.
			{
				auto NewItem = GetWorld()->SpawnActor(AItem::StaticClass());
				NewItem->AttachToActor(Player, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
			}
			else
			{
				// 이미 아이템 붙어있으면 AddBall
				if(OutActors.IsValidIndex(0)) Cast<AItem>(OutActors[0])->AddBall();
			}
		}
		Destroy();
	}
}

