// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BobConsumableActor.h"
#include "BobHealthPotion.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API ABobHealthPotion : public ABobConsumableActor
{
	GENERATED_BODY()
	
protected:

	ABobHealthPotion();

	UPROPERTY(EditAnywhere)
	int32 CreditCost;

	UPROPERTY(EditAnywhere)
	float HealthIncrease;

	void Interact_Implementation(APawn* InstigatorPawn) override;

	FText GetInteractText_Implementation(APawn* InstigatorPawn) override;
};
