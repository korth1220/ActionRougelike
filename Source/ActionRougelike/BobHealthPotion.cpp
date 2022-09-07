// Fill out your copyright notice in the Description page of Project Settings.


#include "BobHealthPotion.h"
#include "BobAttributeComponent.h"
#include "BobPlayerState.h"

#define LOCTEXT_NAMESPACE "InteractableActors" // Turns NSLOCTEXT into LOCTEXT so that it is easier to copy around. (uses the namespace identifier which is usually the first NSLOCTEXT param)

ABobHealthPotion::ABobHealthPotion()
{
	CreditCost = -5;
	CooldownDuration = 3.0f;
	HealthIncrease = 100.0f;
}



void ABobHealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	// Cast<UBobAttributeComponent>(InstigatorPawn->GetComponentByClass(UBobAttributeComponent::StaticClass()));
	UBobAttributeComponent* AttributeComp = UBobAttributeComponent::GetAttributes(InstigatorPawn);
	if (ensure(AttributeComp))
	{
		if (AttributeComp->IsAlive() && AttributeComp->GetHealth() < AttributeComp->GetHealthMax())
		{
			if (ABobPlayerState* InstigatorState = Cast<ABobPlayerState>(InstigatorPawn->GetPlayerState()))
			{
				if (InstigatorState->ChangeCredits(CreditCost))
				{
					AttributeComp->ApplyHeathChange(this, HealthIncrease);
					UseConsumable();
				}
			}
		}
	}
}

FText ABobHealthPotion::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	UBobAttributeComponent* AttributeComp = UBobAttributeComponent::GetAttributes(InstigatorPawn);

	if (AttributeComp && (AttributeComp->GetHealth() == AttributeComp->GetHealthMax()))
	{
		return LOCTEXT("HealthPotion_FullHealthWarning", "Already at full health.");
	}
	return FText::Format(LOCTEXT("HealthPotion_InteractMessage", "Costs {0} Credits. Restores 100 Health."), -CreditCost);

} 

#undef LOCTEXT_NAMESPACE // Turns NSLOCTEXT into LOCTEXT so that it is easier to copy around.