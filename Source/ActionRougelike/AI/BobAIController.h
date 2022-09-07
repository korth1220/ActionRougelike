// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BobAIController.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API ABobAIController : public AAIController
{
	GENERATED_BODY()


protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	UBehaviorTree* BehaviorTree;

};
