// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BobPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPawnChanged, APawn*, NewPawn);

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChanged, APlayerState*, NewPlayerState);

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API ABobPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PauseMenuClass;

	UPROPERTY()
	UUserWidget* PauseMenuInstance;

	UPROPERTY(BlueprintAssignable)
	FOnPawnChanged OnPawnChanged;

	//UPROPERTY(BlueprintAssignable)
	//FOnPlayerStateChanged OnPlayerStateReceived;


	virtual void SetPawn(APawn* InPawn) override;

	virtual void BeginPlayingState() override; // This will solve the issue where the UI is loaded in before the player state is, thus causing things like Credits to not update.

	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintBeginPlayingState();

	//void OnRep_PlayerState() override;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void TogglePauseMenu();

	void SetupInputComponent() override; // Allows us to bind buttons not related to the character.
};

