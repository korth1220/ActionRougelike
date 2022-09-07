// Fill out your copyright notice in the Description page of Project Settings.


#include "BobPlayerController.h"
#include <Blueprint/UserWidget.h>

void ABobPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	OnPawnChanged.Broadcast(InPawn);
}

void ABobPlayerController::BeginPlayingState()
{
	BlueprintBeginPlayingState();
}

void ABobPlayerController::TogglePauseMenu()
{
	if (PauseMenuInstance && PauseMenuInstance->IsInViewport())
	{
		PauseMenuInstance->RemoveFromParent();
		PauseMenuInstance = nullptr;

		bShowMouseCursor = false; // This is a variable inside the player controller, so we can access it here.
		SetInputMode(FInputModeGameOnly()); // Also available inside the player controller, uses a struct. 
		return;
	}


	PauseMenuInstance = CreateWidget<UUserWidget>(this, PauseMenuClass); // FName is optional

	if (PauseMenuInstance)
	{
		PauseMenuInstance->AddToViewport(100); // ZOrder is what layer this widget would be on, setting it at 100 puts it (potentially) above everything else.

		bShowMouseCursor = true; // This is a variable inside the player controller, so we can access it here.
		SetInputMode(FInputModeUIOnly()); // Also available inside the player controller, uses a struct. 
	}
}

void ABobPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	InputComponent->BindAction("PauseMenu", IE_Pressed, this, &ABobPlayerController::TogglePauseMenu);
}

/*void ABobPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	OnPlayerStateReceived.Broadcast(PlayerState);
}*/
