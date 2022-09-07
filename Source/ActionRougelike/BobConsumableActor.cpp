// Fill out your copyright notice in the Description page of Project Settings.


#include "BobConsumableActor.h"
#include <Components/SphereComponent.h>
#include <Net/UnrealNetwork.h>

// Sets default values
ABobConsumableActor::ABobConsumableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	RootComponent = SphereComp;
	SphereComp->SetCollisionProfileName("Consumable");

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("BaseMesh"); // remove this for consumable base class
	BaseMesh->SetupAttachment(SphereComp);
	BaseMesh->SetCollisionProfileName("Consumable");

	CooldownDuration = 5.0f;

	bUseable = true;
	
	bReplicates = true;
}

// Called when the game starts or when spawned
/*void ABobConsumableActor::BeginPlay()
{
	Super::BeginPlay();
	
}*/

void ABobConsumableActor::ConsumableVisibility(bool B)
{
	SetActorEnableCollision(B);
	SphereComp->SetVisibility(B, true);
}

void ABobConsumableActor::CooldownElapsed()
{
	bUseable = true;
	OnRep_Use();
}

void ABobConsumableActor::OnRep_Use()
{
	ConsumableVisibility(bUseable); // @fixme: This being saved may cause it to be unusable if it is saved false due to this timer not activating on the server.
	if (!bUseable)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ConsumableCooldown, this, &ABobConsumableActor::CooldownElapsed, CooldownDuration);
	}
}

// Called every frame
/*void ABobConsumableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}*/

void ABobConsumableActor::UseConsumable()
{
	bUseable = false;
	OnRep_Use();
}

FText ABobConsumableActor::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	return FText::GetEmpty();
}

void ABobConsumableActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABobConsumableActor, bUseable);
}
