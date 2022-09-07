// Fill out your copyright notice in the Description page of Project Settings.


#include "BobAttributeComponent.h"
#include "BobGameModeBase.h"
#include <Net/UnrealNetwork.h>

static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("bob.DamageMult"), 1.0f, TEXT("ALL damage is multiplied by this modifier"), ECVF_Cheat); // Lecture 15: Console Variables

// Sets default values for this component's properties
UBobAttributeComponent::UBobAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//PrimaryComponentTick.bCanEverTick = true;

	MaxHealth = 150.0f;
	Health = MaxHealth; // Start game with full HP

	MaxRage = 100.0f;
	Rage = 0.0f; // Start game with no rage

	SetIsReplicatedByDefault(true);
}

UBobAttributeComponent* UBobAttributeComponent::GetAttributes(AActor* FromActor)
{
	if (FromActor)
	{
		return Cast<UBobAttributeComponent>(FromActor->GetComponentByClass(UBobAttributeComponent::StaticClass()));
	}
	return nullptr;
}

bool UBobAttributeComponent::IsActorAlive(AActor* Actor)
{
	UBobAttributeComponent* AttComp = GetAttributes(Actor);
	if (AttComp)
	{
		return AttComp->IsAlive();
	}
	return false;
}

bool UBobAttributeComponent::ApplyHeathChange(AActor* InstigatorActor, float delta)
{
	if (!GetOwner()->CanBeDamaged() && delta < 0.0f)
	{
		return false;
	}
	// Purpose of this was to implement the thorns buff, code is depreciated
	/*if (GetOwner()->HasAuthority())
	{
		UBobActionComponent* ActionComp = Cast<UBobActionComponent>(InstigatorActor->GetComponentByClass(UBobActionComponent::StaticClass()));
		// This cast is probably inefficient.
		if (ActionComp && ActionComp->ActiveGameplayTags.HasTag(ThornsTag))
		{
			return;
		}
	}*/

	if (delta < 0.0f)
	{
		delta *= CVarDamageMultiplier.GetValueOnGameThread();
	}

	float ActualDelta = Health;
	float NewHealth = FMath::Clamp(Health + delta, 0.0f, MaxHealth);
	ActualDelta = NewHealth - ActualDelta;

	// Only allow HP to change if this instance is on the server.
	if (GetOwner()->HasAuthority())
	{
		Health = NewHealth;

		if (ActualDelta != 0.0f)
		{
			MulticastHealthChanged(InstigatorActor, Health, ActualDelta);
			if (ActualDelta < 0.0f)
			{
				float RageDelta = abs(ActualDelta * 0.5);
				ApplyRageChange(InstigatorActor, RageDelta);
			}
		}

		// Called from server, it automatically calls itself. Called from client, it is local. <- somewhat unrelated statements
		if (ActualDelta < 0.0f && Health == 0.0f)
		{
			ABobGameModeBase* GM = GetWorld()->GetAuthGameMode<ABobGameModeBase>();
			if (GM)
			{
				GM->OnActorKilled(GetOwner(), InstigatorActor);
			}
		}
	}

	return ActualDelta != 0;
}

bool UBobAttributeComponent::ApplyRageChange(AActor* InstigatorActor, float delta)
{
	float ActualDelta = Rage;
	float NewRage = FMath::Clamp(Rage + delta, 0.0f, MaxRage);
	ActualDelta = NewRage - ActualDelta;

	// Only allow HP to change if this instance is on the server.
	if (GetOwner()->HasAuthority())
	{
		Rage = NewRage;

		if (ActualDelta != 0.0f)
		{
			MulticastRageChanged(InstigatorActor, Rage, ActualDelta);
		}
	}

	return ActualDelta != 0;
}

void UBobAttributeComponent::MulticastHealthChanged_Implementation(AActor* InstigatorActor, float NewHealth, float delta)
{
	OnHealthChanged.Broadcast(InstigatorActor, this, NewHealth, delta);
}


void UBobAttributeComponent::MulticastRageChanged_Implementation(AActor* InstigatorActor, float NewRage, float delta)
{
	OnRageChanged.Broadcast(InstigatorActor, this, NewRage, delta);
}

bool UBobAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}

// -- Get START -- 

float UBobAttributeComponent::GetHealth() const
{
	return Health;
}

float UBobAttributeComponent::GetHealthMax() const
{
	return MaxHealth;
}

float UBobAttributeComponent::GetRage() const
{
	return Rage;
}

float UBobAttributeComponent::GetRageMax() const
{
	return MaxRage;
}

// -- Get END --

void UBobAttributeComponent::Kill(AActor* Killer)
{
	ApplyHeathChange(Killer, -MaxHealth);
}

void UBobAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBobAttributeComponent, Health);
	DOREPLIFETIME(UBobAttributeComponent, MaxHealth);

	DOREPLIFETIME(UBobAttributeComponent, Rage);
	DOREPLIFETIME(UBobAttributeComponent, MaxRage);

	// Can use this function to optimize bandwidth by reducing the amount of data sent if it isn't needed
	//DOREPLIFETIME_CONDITION(UBobAttributeComponent, MaxHealth, COND_OwnerOnly);
} 