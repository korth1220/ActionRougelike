// Fill out your copyright notice in the Description page of Project Settings.


#include "BobActionComponent.h"
#include "BobAction.h"
#include "ActionRougelike.h"
#include <Net/UnrealNetwork.h>
#include <Engine/ActorChannel.h>
#include <Components/ActorComponent.h>


DECLARE_CYCLE_STAT(TEXT("StartActionByName"), STAT_StartActionByName, STATGROUP_STANFORD); // Lecture 28, Stat Commands


// Sets default values for this component's properties
UBobActionComponent::UBobActionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	SetIsReplicatedByDefault(true);

}


// Called when the game starts
void UBobActionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...


	if (GetOwner()->HasAuthority())
	{
		for (TSubclassOf<UBobAction> ActionClass : DefaultActions)
		{
			AddAction(GetOwner(), ActionClass);
		}
	}
}

void UBobActionComponent::EndPlay(const EEndPlayReason::Type EndplayReason)
{   // Lecture 28, Fixing Errors
	// This end play is to remove any actions still on a target after they are killed so that they aren't sitting in memory running nothing.

	TArray<UBobAction*> ActionCopy = Actions; // Without making a copied reference, this would crash because removing the action would change the size of the array we are iterating from.
	for (UBobAction* Action : ActionCopy)
	{
		if (Action && Action->IsRunning())
		{
			Action->StopAction(GetOwner());
		}
	}

	Super::EndPlay(EndplayReason);
}




// Called every frame
void UBobActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	//Lists out the gameplay tags currently active on all units with an action component.
	//FString DebugMsg = GetNameSafe(GetOwner()) + " : " + ActiveGameplayTags.ToStringSimple();
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, DebugMsg);

	// Draw All Actions
	// @fixme: Move this to a console command.

 	/*
	for (UBobAction* Action : Actions)
 	{
		FColor TextColor = Action->IsRunning() ? FColor::Blue : FColor::White;
 		FString ActionMsg = FString::Printf(TEXT("[%s] Action: %s"), *GetNameSafe(GetOwner()), *GetNameSafe(Action));
 
		LogOnScreen(this, ActionMsg, TextColor, 0.0f);
	}
	*/

}




void UBobActionComponent::AddAction(AActor* Instigator,TSubclassOf<UBobAction> ActionClass)
{
	if (!ensure(ActionClass))
	{
		return;
	}

	if (!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Client attempting to add action, [Class: %s]"), *GetNameSafe(ActionClass));
		return;
	}


	UBobAction* NewAction = NewObject<UBobAction>(GetOwner(), ActionClass);
	if (ensure(NewAction))
	{
		NewAction->Init(this);
		Actions.Add(NewAction);
		// Lecture 18: Creating Buffs 13:00

		if (NewAction->bAutoStart && ensure(NewAction->CanStart(Instigator)))
		{
			NewAction->StartAction(Instigator);
		}
	}
	// Lecture 16: Starting our Action System

}

void UBobActionComponent::RemoveActon(UBobAction* ActionToRemove)
{
	if (ensure(ActionToRemove && !ActionToRemove->IsRunning()))
		return;

	Actions.Remove(ActionToRemove);
}

bool UBobActionComponent::StartActionByName(AActor* Instigator, FName ActionName)
{
	SCOPE_CYCLE_COUNTER(STAT_StartActionByName); // Lecture 28, Stat Commands

	for (UBobAction* Action : Actions)
	{
		if (Action && Action->ActionName == ActionName && !Action->IsRunning())
		{
			if (!Action->CanStart(Instigator))
			{
				continue;
			}

			if (!GetOwner()->HasAuthority())
			{
				ServerStartAction(Instigator, ActionName);
			}

			Action->StartAction(Instigator);
			return true;
		}
	}
	return false;
}

bool UBobActionComponent::StopActionByName(AActor* Instigator, FName ActionName)
{
	for (UBobAction* Action : Actions)
	{
		
		if (Action && Action->ActionName == ActionName)
		{
			if (Action->IsRunning())
			{
				if (!GetOwner()->HasAuthority())
				{
					ServerStopAction(Instigator, ActionName);
				}

				Action->StopAction(Instigator);
				return true;
			}
		}
	}
	return false;
}

void UBobActionComponent::ServerStartAction_Implementation(AActor* Instigator, FName ActionName)
{
	StartActionByName(Instigator, ActionName);
}

void UBobActionComponent::ServerStopAction_Implementation(AActor* Instigator, FName ActionName)
{
	StopActionByName(Instigator, ActionName);
}

void UBobActionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	DOREPLIFETIME(UBobActionComponent, Actions);
}

bool UBobActionComponent::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	for (UBobAction* Action : Actions)
	{
		if (Action)
		{
			WroteSomething |= Channel->ReplicateSubobject(Action, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}