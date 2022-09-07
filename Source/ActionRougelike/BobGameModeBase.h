// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include <AIModule/Classes/EnvironmentQuery/EnvQueryTypes.h>
#include <Engine/DataTable.h>
#include "BobGameModeBase.generated.h"

class UEnvQuery;
class UEnvQueryInstanceBlueprintWrapper;
class UCurveFloat;
class UBobSaveGame;
class UDataTable; // When dealing with data tables you need to associate them with a struct
class UBobMinionDataAsset;


USTRUCT(BlueprintType) // Exposed to blueprint so we can edit it there
struct FMonsterInfoRow : public FTableRowBase // Derived from FTableRowBase so that it can be associated with data tables, needs to be included
{
	GENERATED_BODY()
public:
	
	FMonsterInfoRow()
	{
		Weight = 1.0f;
		SpawnCost = 5.0f;
		KillReward = 20.0f; 
	}


	//UPROPERTY(EditAnywhere, BlueprintReadOnly)
	//UBobMinionDataAsset* MonsterData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPrimaryAssetId MonsterId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Weight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnCost;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float KillReward;
};

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API ABobGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	ABobGameModeBase();

	virtual void StartPlay() override;

	virtual void OnActorKilled(AActor* ActorKilled, AActor* Killer);

	UFUNCTION(Exec)
	void KillAll();

	UFUNCTION(BlueprintCallable, Category = "Savegame")
	void WriteSaveGame();

	UFUNCTION(BlueprintCallable, Category = "Savegame")
	void ReadSaveGame();

	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

protected:

	FTimerHandle TimerHandle_SpawnBots;

	//UPROPERTY(EditDefaultsOnly, Category = "AI")
	//TSubclassOf<AActor> MinionClass;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UEnvQuery* SpawnBotQuery;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UCurveFloat* DifficultyCurve;

		
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UDataTable* MonsterTable;


	UPROPERTY()
	UBobSaveGame* CurrentSave;

	FString SlotName;

	UFUNCTION()
	void RespawnPlayerElapsed(AController* Cont);

	UFUNCTION()
	void SpawnBotsTimerElapsed();

	UFUNCTION()
	void OnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float SpawnTimerInterval;

	void OnMonsterLoaded(FPrimaryAssetId LoadedId, FVector SpawnLocation);

};
