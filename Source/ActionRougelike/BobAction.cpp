// Fill out your copyright notice in the Description page of Project Settings.


#include "BobAction.h"
#include "BobActionComponent.h"
#include "ActionRougelike.h"
#include <Net/UnrealNetwork.h>

void UBobAction::Init(UBobActionComponent* AC)
{
	ActionComp = AC;
}

bool UBobAction::CanStart_Implementation(AActor* Instigator)
{
	UBobActionComponent* Comp = GetOwningComponent();
	if (Comp->ActiveGameplayTags.HasAny(BlockedTags) || IsRunning())
	{
		return false;
	}
	return true;
}

void UBobAction::StartAction_Implementation(AActor* Instigator)
{
	// This log code is for programs without LogOnScreen
	//UE_LOG(LogTemp, Log, TEXT("Running: %s"), *GetNameSafe(this));

	//LogOnScreen(this, FString::Printf(TEXT("Started: %s"), *ActionName.ToString()), FColor::Green);


	UBobActionComponent* Comp = GetOwningComponent();

	Comp->ActiveGameplayTags.AppendTags(GrantsTags);

	RepData.bIsRunning = true;
	RepData.Instigator = Instigator;

	if (GetOwningComponent()->GetOwnerRole() == ROLE_Authority)
	{
		TimeStarted = GetWorld()->TimeSeconds;
	}

	Comp->OnActionStarted.Broadcast(Comp, this);
}

void UBobAction::StopAction_Implementation(AActor* Instigator)
{
	// This log code is for programs without LogOnScreen
	//UE_LOG(LogTemp, Log, TEXT("Stopping: %s"), *GetNameSafe(this));

	//LogOnScreen(this, FString::Printf(TEXT("Stopped: %s"), *ActionName.ToString()), FColor::White);


	//ensureAlways(bIsRunning);

	UBobActionComponent* Comp = GetOwningComponent();

	Comp->ActiveGameplayTags.RemoveTags(GrantsTags);

	RepData.bIsRunning = false;
	RepData.Instigator = Instigator;

	Comp->OnActionStopped.Broadcast(Comp, this);
}



UWorld* UBobAction::GetWorld() const
{ // Lecture 16: Projectile Attack

	// Outer is set when making actions with NewObject<T>
	AActor* Actor = Cast<AActor>(GetOuter());
	if (Actor)
	{
		return Actor->GetWorld();
	}
	return nullptr;
}

UBobActionComponent* UBobAction::GetOwningComponent() const
{
	//return Cast<UBobActionComponent>(GetOuter());

	return ActionComp;
}

void UBobAction::OnRep_RepData()
{
	if (RepData.bIsRunning)
	{
		StartAction(RepData.Instigator);
	}
	else // Putting the instigator and is running into a struct makes sure they both arrive at the same time.
	{
		StopAction(RepData.Instigator);
	}
}

bool UBobAction::IsRunning() const
{
	return RepData.bIsRunning;
}



void UBobAction::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	DOREPLIFETIME(UBobAction, RepData);
	DOREPLIFETIME(UBobAction, TimeStarted);
	DOREPLIFETIME(UBobAction, ActionComp);
}
