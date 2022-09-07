// Fill out your copyright notice in the Description page of Project Settings.


#include "BobAnimInstance.h"
#include <GameplayTagContainer.h>
#include "BobActionComponent.h"

void UBobAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	AActor* OwningActor = GetOwningActor();
	if (OwningActor)
	{
		ActionComp = Cast<UBobActionComponent>(OwningActor->GetComponentByClass(UBobActionComponent::StaticClass()));
	}

}

void UBobAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	static FGameplayTag StunnedTag = FGameplayTag::RequestGameplayTag("Status.Stunned");



	if (ActionComp)
	{
		bIsStunned = ActionComp->ActiveGameplayTags.HasTag(StunnedTag);
	}

}
