// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BobPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCreditsChanged, ABobPlayerState*, PlayerState, int32, NewCredits, int32, delta);

class UBobSaveGame;

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API ABobPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	ABobPlayerState();

	UFUNCTION(BlueprintCallable, Category = "Score")
	int32 GetCredits();

	UFUNCTION(BlueprintCallable, Category = "Score")
	bool ChangeCredits(int32 Cred);
	
	//UFUNCTION(BlueprintCallable, Category = "Score")
	//void RemoveCredits(int32 Cred);

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnCreditsChanged OnCreditsChanged;

	UFUNCTION()
	void OnRep_Credits(int32 OldCredits);


	UFUNCTION(BlueprintNativeEvent)
	void SavePlayerState(UBobSaveGame* SaveObject);

	UFUNCTION(BlueprintNativeEvent)
	void LoadPlayerState(UBobSaveGame* SaveObject);

protected:

	UPROPERTY(EditDefaultsOnly, ReplicatedUsing="OnRep_Credits", Category = "Score")
	int32 Credits;
};
