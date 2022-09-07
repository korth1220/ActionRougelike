// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BobAnimInstance.generated.h"

class UBobActionComponent;

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API UBobAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	bool bIsStunned;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	UBobActionComponent* ActionComp;

	void NativeInitializeAnimation() override; // Event Begin Play

	void NativeUpdateAnimation(float DeltaSeconds) override; // Event Tick

};
