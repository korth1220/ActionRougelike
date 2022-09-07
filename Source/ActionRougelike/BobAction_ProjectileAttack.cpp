// Fill out your copyright notice in the Description page of Project Settings.


#include "BobAction_ProjectileAttack.h"
#include <GameFramework/Character.h>
#include <Kismet/GameplayStatics.h>

UBobAction_ProjectileAttack::UBobAction_ProjectileAttack()
{
	HandSocketName = "Muzzle_01";
	AttackAnimDelay = 0.2f;
}

void UBobAction_ProjectileAttack::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	ACharacter* Char = Cast<ACharacter>(Instigator);
	if (!Char)
	{
		return;
	}

	Char->PlayAnimMontage(AttackAnim);
	UGameplayStatics::SpawnEmitterAttached(CastingEffect, Char->GetMesh(), HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);

	if (Char->HasAuthority())
	{
		FTimerHandle TimerHandle_AttackDelay;
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "AttackDelayElapsed", Char);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackDelay, Delegate, AttackAnimDelay, false);
	}
}

void UBobAction_ProjectileAttack::AttackDelayElapsed(ACharacter* InstigatorCharacter)
{
	if (!ensureAlways(ProjectileClass))
	{
		StopAction(InstigatorCharacter);
		return;
	}
	
	FVector HandLocation = InstigatorCharacter->GetMesh()->GetSocketLocation("Muzzle_01");

	FVector CamLoc = InstigatorCharacter->GetPawnViewLocation();

	FCollisionObjectQueryParams QParams;
	QParams.AddObjectTypesToQuery(ECC_WorldStatic);
	QParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	FHitResult Hit;
	FVector End = CamLoc + (InstigatorCharacter->GetControlRotation().Vector() * 5000);

	if (GetWorld()->LineTraceSingleByObjectType(Hit, CamLoc, End, QParams))
	{
		End = Hit.ImpactPoint;
	}

	FRotator TraceRot = FRotationMatrix::MakeFromX(End - HandLocation).Rotator();

	FTransform SpawnTM = FTransform(TraceRot, HandLocation);


	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = InstigatorCharacter;


	GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
	StopAction(InstigatorCharacter);
}
