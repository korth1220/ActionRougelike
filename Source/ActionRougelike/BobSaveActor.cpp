// Fill out your copyright notice in the Description page of Project Settings.


#include "BobSaveActor.h"
#include <Particles/ParticleSystemComponent.h>
#include <Net/UnrealNetwork.h>

// Sets default values
ABobSaveActor::ABobSaveActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("BaseMesh");
	RootComponent = BaseMesh;

	FireParticles = CreateDefaultSubobject<UParticleSystemComponent>("FireParticles");
	FireParticles->SetupAttachment(BaseMesh);
	FireParticles->bAutoActivate = false;

	bReplicates = true;
}

void ABobSaveActor::OnRep_FireActivated()
{
	if (bFireActivated)
	{
		FireParticles->ActivateSystem();
	}
	else
	{
		FireParticles->DeactivateSystem();
	}
}

void ABobSaveActor::OnActorLoaded_Implementation()
{
	OnRep_FireActivated();
}

void ABobSaveActor::Interact_Implementation(APawn* InstigatorPawn)
{
	bFireActivated = !bFireActivated; // On a normal save game object this would not reset, but it does here for testing purposes.
	OnRep_FireActivated();
}

void ABobSaveActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABobSaveActor, bFireActivated);
}
