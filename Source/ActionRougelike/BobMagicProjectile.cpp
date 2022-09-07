// Fill out your copyright notice in the Description page of Project Settings.


#include "BobMagicProjectile.h"
#include <Components/SphereComponent.h>
#include <Particles/ParticleSystemComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>
#include "BobAttributeComponent.h"
#include <Components/AudioComponent.h>
#include "BobGameplayFunctionLibrary.h"
#include "BobActionComponent.h"
#include "BobActionEffect.h"

// Sets default values
ABobMagicProjectile::ABobMagicProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetCollisionProfileName("Projectile");
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ABobMagicProjectile::OnActorOverlap);
	RootComponent = SphereComp;

	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>("EffectComp");
	EffectComp->SetupAttachment(SphereComp);
	EffectComp->SetCollisionProfileName("Projectile");

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComp");
	MovementComp->InitialSpeed = 3000.0f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bInitialVelocityInLocalSpace = true;

	FlightLoopComp = CreateDefaultSubobject<UAudioComponent>("FlightLoopComp");
	FlightLoopComp->SetupAttachment(RootComponent);

	Damage = -20.0f;

	bReplicates = true;
}


void ABobMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != GetInstigator())
	{
		
		UBobActionComponent* ActionComp = Cast<UBobActionComponent>(OtherActor->GetComponentByClass(UBobActionComponent::StaticClass()));
		
		if (ActionComp && ActionComp->ActiveGameplayTags.HasTag(ParryTag))
		{
			MovementComp->Velocity = -MovementComp->Velocity;
			SetInstigator(Cast<APawn>(OtherActor));
			return;
		}
		
		
		
		
		/*UBobAttributeComponent* AttributeComp = UBobAttributeComponent::GetAttributes(OtherActor);
		if (AttributeComp && ensure(IsValid(this)))
		{
			AttributeComp->ApplyHeathChange(GetInstigator(), Damage);
			// destroy projectile is set in BP
		}*/ 
		// depreciated code for dealing damage, now located in gameplayfunctionlibrary

		if (ensure(IsValid(this)))
		{
			//if (GetOwner()->HasAuthority()) // CRASH ----> Unknown bug, needs heavy review
			//{
				if (UBobGameplayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(), OtherActor, Damage, SweepResult))
				{
					ActionComp->AddAction(GetInstigator(), BurningActionClass);
				}
			//}
		}
		

	}
}


