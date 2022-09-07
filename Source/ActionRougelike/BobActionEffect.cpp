// Fill out your copyright notice in the Description page of Project Settings.


#include "BobActionEffect.h"
#include "BobActionComponent.h"
#include <GameFramework/GameStateBase.h>

UBobActionEffect::UBobActionEffect()
{
	bAutoStart = true;
}



void UBobActionEffect::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	if (Duration > 0.0f)
	{
		FTimerDelegate Delegate;

		Delegate.BindUFunction(this, "StopAction", Instigator);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Duration, Delegate, Duration, false);

	}

	if (Period > 0.0f)
	{
		FTimerDelegate Delegate;

		Delegate.BindUFunction(this, "ExecutePeriodicEffect", Instigator);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Period, Delegate, Period, true);

	}
}

void UBobActionEffect::StopAction_Implementation(AActor* Instigator)
{ 
	if (GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle_Period) < KINDA_SMALL_NUMBER)
	{
		ExecutePeriodicEffect(Instigator);
	}


	Super::StopAction_Implementation(Instigator);

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Duration);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Period);

	UBobActionComponent* ActComp = GetOwningComponent();
	if (ActComp)
	{
		ActComp->RemoveActon(this);
	}


}

void UBobActionEffect::ExecutePeriodicEffect_Implementation(AActor* Instigator)
{

}

float UBobActionEffect::GetTimeRemaining() const
{
	AGameStateBase* GS = GetWorld()->GetGameState<AGameStateBase>();

	if (GS)
	{
		float EndTime = TimeStarted + Duration;
		return EndTime - GS->GetServerWorldTimeSeconds();
	}
	return Duration;
}