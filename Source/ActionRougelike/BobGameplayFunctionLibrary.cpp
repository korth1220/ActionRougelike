// Fill out your copyright notice in the Description page of Project Settings.


#include "BobGameplayFunctionLibrary.h"
#include "BobAttributeComponent.h"

bool UBobGameplayFunctionLibrary::ApplyDamage(AActor* DamageSource, AActor* TargetActor, float DamageMagnitude)
{
	UBobAttributeComponent* AttComp = UBobAttributeComponent::GetAttributes(TargetActor);
	if (AttComp)
	{
		return AttComp->ApplyHeathChange(DamageSource, DamageMagnitude);
	}
	return false;
}

bool UBobGameplayFunctionLibrary::ApplyDirectionalDamage(AActor* DamageSource, AActor* TargetActor, float DamageMagnitude, const FHitResult& HitResult)
{
	if (ApplyDamage(DamageSource, TargetActor, DamageMagnitude))
	{
		UPrimitiveComponent* HitComp = HitResult.GetComponent();
		if (HitComp && HitComp->IsSimulatingPhysics(HitResult.BoneName))
		{
			FVector Direction = HitResult.TraceEnd - HitResult.TraceStart;
			Direction.Normalize();

			HitComp->AddImpulseAtLocation(Direction * 300000.0f, HitResult.ImpactPoint, HitResult.BoneName);
		}
		return true;
	}
	return false;
}
