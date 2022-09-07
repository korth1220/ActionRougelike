// Fill out your copyright notice in the Description page of Project Settings.


#include "BobExplosiveBarrel.h"
#include <PhysicsEngine/RadialForceComponent.h>
#include <Components/StaticMeshComponent.h>

// Sets default values
ABobExplosiveBarrel::ABobExplosiveBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComp");
	StaticMeshComp->SetSimulatePhysics(true);
	StaticMeshComp->SetCollisionProfileName("PhysicsActor");
	StaticMeshComp->SetMassOverrideInKg("StaticMeshComp",50.0f,true);
	RootComponent = StaticMeshComp;

	ForceComp = CreateDefaultSubobject<URadialForceComponent>("ForceComp");
	ForceComp->SetupAttachment(StaticMeshComp);
	ForceComp->ImpulseStrength = 60000.0f;
	ForceComp->Radius = 750.0f;

	ForceComp->AddCollisionChannelToAffect(ECC_WorldDynamic);

	//StaticMeshComp->OnComponentHit.AddDynamic(this, &ABobExplosiveBarrel::OnActorHit);
}


void ABobExplosiveBarrel::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	StaticMeshComp->OnComponentHit.AddDynamic(this, &ABobExplosiveBarrel::OnActorHit);
}



void ABobExplosiveBarrel::OnActorHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ForceComp->FireImpulse();
}

