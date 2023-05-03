// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDrop.h"
#include "Characters/TPSPlayer.h"
#include "Item.h"

#include <Components/BoxComponent.h>
#include <Particles/ParticleSystem.h>
#include <Particles/ParticleSystemComponent.h>
#include <Kismet/GameplayStatics.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(ItemDrop)

AItemDrop::AItemDrop()
{
	PrimaryActorTick.bCanEverTick = false;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetCollisionProfileName(TEXT("ItemDrop"));
	SetRootComponent(Box);

	ItemPreview = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SkillPreview"));
	ConstructorHelpers::FObjectFinder<UParticleSystem> EmitterTemp(TEXT("/Script/Engine.ParticleSystem'/Game/Effects/P_ItemBall.P_ItemBall'"));
	if (EmitterTemp.Succeeded())
	{
		ItemPreview->SetTemplate(EmitterTemp.Object);
		ItemPreview->SetRelativeScale3D(FVector(.8f));
	}
	ItemPreview->SetupAttachment(RootComponent);

	ConstructorHelpers::FObjectFinder<UParticleSystem> OverlapEmitterTemp(TEXT("/Script/Engine.ParticleSystem'/Game/Effects/P_ItemOverlap.P_ItemOverlap'"));
	if (OverlapEmitterTemp.Succeeded()) { OverlapEmitter = OverlapEmitterTemp.Object; }

	ConstructorHelpers::FObjectFinder<USoundWave> ItemOverlapSoundTemp(TEXT("/Script/Engine.SoundWave'/Game/Effects/Sounds/ItemDropOverlap.ItemDropOverlap'"));
	if (ItemOverlapSoundTemp.Succeeded()) { ItemOverlapSound = ItemOverlapSoundTemp.Object; }
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
	auto Player = Cast<ATPSPlayer>(OtherActor);
	if (Player != nullptr)
	{
		if (OverlapEmitter != nullptr) UGameplayStatics::SpawnEmitterAttached(OverlapEmitter, Player->GetMesh(), NAME_None, FVector(ForceInit), FRotator(ForceInit), FVector(0.5f, 0.5f, 1.f));
		if (ItemOverlapSound != nullptr) UGameplayStatics::PlaySoundAtLocation(GetWorld(), ItemOverlapSound, GetActorLocation());

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
			if (OutActors.IsValidIndex(0)) Cast<AItem>(OutActors[0])->AddBall();
		}
	}
	Destroy();
}

