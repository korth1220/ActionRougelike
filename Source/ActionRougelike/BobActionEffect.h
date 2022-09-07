// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BobAction.h"
#include "BobActionEffect.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API UBobActionEffect : public UBobAction
{
	GENERATED_BODY()
	
public:

	UBobActionEffect();

	void StartAction_Implementation(AActor* Instigator) override;

	void StopAction_Implementation(AActor* Instigator) override;

	UFUNCTION(BlueprintCallable, Category = "Action")
	float GetTimeRemaining() const; // Getter function, can be labeled as const

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Buff")
	float Duration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Buff")
	float Period;

	FTimerHandle TimerHandle_Duration;
	FTimerHandle TimerHandle_Period;

	UFUNCTION(BlueprintNativeEvent, Category = "Buff")
	void ExecutePeriodicEffect(AActor* Instigator);
};
