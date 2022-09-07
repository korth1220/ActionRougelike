// Fill out your copyright notice in the Description page of Project Settings.


#include "BobPlayerState.h"
#include "BobSaveGame.h"
#include <Net/UnrealNetwork.h>


ABobPlayerState::ABobPlayerState()
{
	bReplicates = true;
}

int32 ABobPlayerState::GetCredits()
{
	return Credits;
}

bool ABobPlayerState::ChangeCredits(int32 delta)
{
	int32 NewCredits = Credits;
	NewCredits = (Credits + delta);
	
	if (NewCredits < 0)
	{
		return false;
	}

	Credits = NewCredits;
	OnCreditsChanged.Broadcast(this, Credits, NewCredits);

	return true;
}

void ABobPlayerState::OnRep_Credits(int32 OldCredits)
{
	OnCreditsChanged.Broadcast(this, Credits, Credits-OldCredits);
}

void ABobPlayerState::SavePlayerState_Implementation(UBobSaveGame* SaveObject)
{
	if (SaveObject)
	{
		SaveObject->Credits = Credits;
	}
}

void ABobPlayerState::LoadPlayerState_Implementation(UBobSaveGame* SaveObject)
{
	if (SaveObject)
	{
		ChangeCredits(SaveObject->Credits);
	}
}

void ABobPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABobPlayerState, Credits); // @fixme: For some reason, this does not update client-side on the HUD
}