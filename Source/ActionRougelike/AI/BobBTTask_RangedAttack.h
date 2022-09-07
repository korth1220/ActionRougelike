// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
//#include <Templates/SubclassOf.h>
#include "BobBTTask_RangedAttack.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API UBobBTTask_RangedAttack : public UBTTaskNode
{
	GENERATED_BODY()
	

protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "AI")
	float MaxBulletSpread;

	UPROPERTY(EditAnywhere, Category = "AI")
	TSubclassOf<AActor> Projectile;

public:
	
	UBobBTTask_RangedAttack();
};
