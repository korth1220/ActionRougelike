// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BobConsumableActor.h"
#include "BobCreditConsumable.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API ABobCreditConsumable : public ABobConsumableActor
{
	GENERATED_BODY()
	
protected:

	ABobCreditConsumable();

	UPROPERTY(EditAnywhere)
	int32 CreditIncrease;

	void Interact_Implementation(APawn* InstigatorPawn) override;

};
