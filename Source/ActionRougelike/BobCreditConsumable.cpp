// Fill out your copyright notice in the Description page of Project Settings.


#include "BobCreditConsumable.h"
#include "BobPlayerState.h"

ABobCreditConsumable::ABobCreditConsumable()
{
	CreditIncrease = 5;
}

void ABobCreditConsumable::Interact_Implementation(APawn* InstigatorPawn)
{
	if (ABobPlayerState* InstigatorState = Cast<ABobPlayerState>(InstigatorPawn->GetPlayerState()))
	{
		if (InstigatorState->ChangeCredits(CreditIncrease))
		{
			UseConsumable();
		}
	}
}
