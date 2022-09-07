// Fill out your copyright notice in the Description page of Project Settings.


#include "BobBTTask_RangedAttack.h"
#include <AIModule/Classes/AIController.h>
#include <GameFramework/Character.h>
#include <AIModule/Classes/BehaviorTree/BlackboardComponent.h>
#include "../BobAttributeComponent.h"

UBobBTTask_RangedAttack::UBobBTTask_RangedAttack()
{
	MaxBulletSpread = 3.0f;
}


EBTNodeResult::Type UBobBTTask_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICont = OwnerComp.GetAIOwner();
	if (ensure(AICont))
	{
		ACharacter* AICharacter = Cast<ACharacter>(AICont->GetPawn());
		if (AICharacter == nullptr)
		{
			return EBTNodeResult::Failed;
		}

		FVector MuzzleLoc = AICharacter->GetMesh()->GetSocketLocation("Muzzle_01");
		
		AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetActor"));
		if (TargetActor == nullptr)
		{
			return EBTNodeResult::Failed;
		}

		if (!UBobAttributeComponent::IsActorAlive(TargetActor))
		{
			return EBTNodeResult::Failed;
		}


		FVector Direction = TargetActor->GetActorLocation() - MuzzleLoc;
		FRotator MuzzleRot = Direction.Rotation();

		MuzzleRot.Pitch += FMath::RandRange(0.0f, MaxBulletSpread);
		MuzzleRot.Yaw += FMath::RandRange(-MaxBulletSpread, MaxBulletSpread);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = AICharacter;


		AActor* SpawnedProjectile = GetWorld()->SpawnActor<AActor>(Projectile, MuzzleLoc, MuzzleRot, SpawnParams);


		return /*SpawnedProjectile ?*/ EBTNodeResult::Succeeded; //: EBTNodeResult::Failed;
	}
	return EBTNodeResult::Failed;
}
