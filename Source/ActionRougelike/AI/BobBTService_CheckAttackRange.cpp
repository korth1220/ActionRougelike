// Fill out your copyright notice in the Description page of Project Settings.


#include "BobBTService_CheckAttackRange.h"
#include <AIModule/Classes/BehaviorTree/BlackboardComponent.h>
#include <AIModule/Classes/AIController.h>

void UBobBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// check distance
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if (ensure(BBComp))
	{
		AActor* TargetActor = Cast<AActor>(BBComp->GetValueAsObject("TargetActor"));
		if (TargetActor)
		{
			AAIController* AICont = OwnerComp.GetAIOwner();
			if (ensure(AICont))
			{
				APawn* AIPawn = AICont->GetPawn();
				if (ensure(AIPawn))
				{
					float DistanceTo = FVector::Distance(AIPawn->GetTargetLocation(), TargetActor->GetTargetLocation());
					bool bWithinRange = DistanceTo < 2000;
					bool bLOS = false;

					if (bWithinRange)
					{
						bLOS = AICont->LineOfSightTo(TargetActor);
					}
				
					BBComp->SetValueAsBool(AttackRangeKey.SelectedKeyName, (bWithinRange && bLOS));
				}


			}
		}
	}

} 
