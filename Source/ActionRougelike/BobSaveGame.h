// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "BobSaveGame.generated.h"


USTRUCT()
struct FActorSaveData
{
	GENERATED_BODY()
public:
	
	UPROPERTY() // Identifies what actor this data belongs to
	FString ActorName;

	UPROPERTY() // Vectors for the actor's saved location/rotation/scale.
	FTransform ActorTransform;

	UPROPERTY() // Takes variables as bytes to save space.
	TArray<uint8> ByteData;
};

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API UBobSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:

	UPROPERTY()
	int32 Credits;

	UPROPERTY()
	TArray<FActorSaveData> SavedActors;

};
