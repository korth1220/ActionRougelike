// Fill out your copyright notice in the Description page of Project Settings.


#include "BobTeleportProjectile.h"
#include <Components/SphereComponent.h>
#include <Particles/ParticleSystemComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>
#include "BobAttributeComponent.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
ABobTeleportProjectile::ABobTeleportProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetCollisionProfileName("Projectile");
	RootComponent = SphereComp;

	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>("EffectComp");
	EffectComp->SetupAttachment(SphereComp);
	EffectComp->SetCollisionProfileName("Projectile");

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComp");
	MovementComp->InitialSpeed = 6000.0f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bInitialVelocityInLocalSpace = true;

	bReplicates = true;
}

// Called when the game starts or when spawned
void ABobTeleportProjectile::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Teleport, this, &ABobTeleportProjectile::TeleportTimerElapsed, 0.2f);
}


void ABobTeleportProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	TeleportTimerElapsed();
}

void ABobTeleportProjectile::TeleportTimerElapsed()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Teleport);

	UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());

	EffectComp->DeactivateSystem();
	MovementComp->StopMovementImmediately();
	SetActorEnableCollision(false);

	FTimerHandle Delay;
	GetWorld()->GetTimerManager().SetTimer(Delay, this, &ABobTeleportProjectile::TeleportAction, 0.1f);
}

void ABobTeleportProjectile::TeleportAction()
{
	AActor* ActorToTeleport = GetInstigator();
	if (ensure(ActorToTeleport)) 
	{
		ActorToTeleport->TeleportTo(GetActorLocation(), ActorToTeleport->GetActorRotation(), false, false);	
	}
}

// Called every frame
void ABobTeleportProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

