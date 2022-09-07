// Fill out your copyright notice in the Description page of Project Settings.


#include "BobItemChest.h"
#include <Net/UnrealNetwork.h>

// Sets default values
ABobItemChest::ABobItemChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("BaseMesh");
	RootComponent = BaseMesh;

	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>("LidMesh");
	LidMesh->SetupAttachment(BaseMesh);

	targetPitch = 110.0;


	bReplicates = true;
}


void ABobItemChest::OnRep_LidOpened()
{
	float CurrPitch = bLidOpened ? targetPitch : 0.0f;
	LidMesh->SetRelativeRotation(FRotator(CurrPitch, 0, 0));
}

void ABobItemChest::OnActorLoaded_Implementation()
{
	OnRep_LidOpened();
}


void ABobItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	bLidOpened = !bLidOpened;
	OnRep_LidOpened();
	// While using RepNotify, after the value is changed you must call it manually or the server will not update (RepNotify sends the call to the clients, not the sever)
	// This does not happen on Blueprint, it calls server automatically
}

void ABobItemChest::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	// Simple condition, sends the value of bLidOpened to all clients when it changes on the server.
	DOREPLIFETIME(ABobItemChest, bLidOpened);
}

