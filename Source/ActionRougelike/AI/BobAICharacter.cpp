// Fill out your copyright notice in the Description page of Project Settings.


#include "BobAICharacter.h"
#include <AIModule/Classes/Perception/PawnSensingComponent.h>
#include <AIModule/Classes/AIController.h>
#include <AIModule/Classes/BehaviorTree/BlackboardComponent.h>
#include "../BobAttributeComponent.h"
#include "BrainComponent.h"
#include <Blueprint/UserWidget.h>
#include "../BobWorldUserWidget.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../BobActionComponent.h"

// Sets default values
ABobAICharacter::ABobAICharacter()
{
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComponent");

	AttributeComp = CreateDefaultSubobject<UBobAttributeComponent>("AttributeComp");

	ActionComp = CreateDefaultSubobject<UBobActionComponent>("ActionComp");
	
	TimeToHitParamName = "TimeToHit";

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);

	GetMesh()->SetGenerateOverlapEvents(true); // @fixme: Add in another collision channel so it doesnt ignore destructible

}

void ABobAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	PawnSensingComp->OnSeePawn.AddDynamic(this, &ABobAICharacter::OnPawnSeen);
	AttributeComp->OnHealthChanged.AddDynamic(this, &ABobAICharacter::OnHealthChanged);
}

void ABobAICharacter::OnHealthChanged(AActor* InstigatorActor, UBobAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if (Delta < 0.0f)
	{
		if (InstigatorActor!=this)
		{
			SetTargetActor(InstigatorActor);
		}


		if (HPWidget == nullptr)
		{
			HPWidget = CreateWidget<UBobWorldUserWidget>(GetWorld(), HealthBarWidgetClass);

			if (HPWidget)
			{
				HPWidget->AttachedActor = this;
				HPWidget->AddToViewport();
			}
		}


		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);


		// Died
		if (NewHealth <= 0.0f)
		{
			//stop logic
			AAIController* AIC = Cast<AAIController>(GetController());
			if (AIC)
			{
				AIC->GetBrainComponent()->StopLogic("Killed");
			}
			
			//rag doll
			GetMesh()->SetAllBodiesSimulatePhysics(true);
			GetMesh()->SetCollisionProfileName("Ragdoll");

			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GetCharacterMovement()->DisableMovement();

			//lifespan
			SetLifeSpan(10.0f);

		}
	}
}

void ABobAICharacter::SetTargetActor(AActor* NewTarget)
{
	AAIController* AIC = Cast<AAIController>(GetController());
	if (AIC)
	{
		AIC->GetBlackboardComponent()->SetValueAsObject("TargetActor", NewTarget);
	}
}

void ABobAICharacter::OnPawnSeen(APawn* Pawn)
{
	SetTargetActor(Pawn);
	//DrawDebugString(GetWorld(), GetActorLocation(), "Player Spotted", nullptr, FColor::White, 4.0f, true);
}

