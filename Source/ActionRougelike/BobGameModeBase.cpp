// Fill out your copyright notice in the Description page of Project Settings.


#include "BobGameModeBase.h"
#include <AIModule/Classes/EnvironmentQuery/EnvQueryManager.h>
#include <AIModule/Classes/EnvironmentQuery/EnvQueryTypes.h>
#include <AIModule/Classes/EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h>
#include "AI/BobAICharacter.h"
#include "BobAttributeComponent.h"
#include <EngineUtils.h>
#include <DrawDebugHelpers.h>
#include "BobCharacter.h"
#include "BobPlayerState.h"
#include <Kismet/GameplayStatics.h>
#include "BobSaveGame.h"
#include <GameFramework/GameStateBase.h>
#include "BobGameplayInterface.h"
#include <Serialization/ObjectAndNameAsStringProxyArchive.h>
#include "BobMinionDataAsset.h"
#include "ActionRougelike.h"
#include "BobActionComponent.h"
#include <Engine/AssetManager.h>

static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("bob.SpawnBots"), false, TEXT("Enable bot spawning from the gamemode"), ECVF_Cheat); // Lecture 15: Console Variables

ABobGameModeBase::ABobGameModeBase()
{

	SpawnTimerInterval = 2.0f;

	SlotName = "Savegame_00";
}

void ABobGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	
	//ReadSaveGame(); // This calls the save game too early, resulting in only some objects being registered in GetWorld()
}

void ABobGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	ABobPlayerState* PS = NewPlayer->GetPlayerState<ABobPlayerState>();
	if (PS)
	{
		PS->LoadPlayerState(CurrentSave);
	} // @fixme: When a player loads into the game they get the current save game, whatever it is at the time.
}

void ABobGameModeBase::StartPlay()
{
	Super::StartPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &ABobGameModeBase::SpawnBotsTimerElapsed, SpawnTimerInterval, true);

}

void ABobGameModeBase::OnActorKilled(AActor* ActorKilled, AActor* Killer)
{
	ABobCharacter* Player = Cast<ABobCharacter>(ActorKilled);
	if (Player)
	{
		FTimerHandle TimerHandle_RespawnDelay;
		FTimerDelegate Delegate; // Lecture 15: Fixing Player Respawns
		Delegate.BindUFunction(this, "RespawnPlayerElapsed", Player->GetController());

		GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, Delegate, 2.0f, false);
	}
	UE_LOG(LogTemp, Log, TEXT("OnActorKilled: Victim - %s | Killer - %s"), *GetNameSafe(ActorKilled), *GetNameSafe(Killer));

	if (APawn* InstigatorPlayer = Cast<APawn>(Killer))
	{
		if (ABobPlayerState* InstigatorState = Cast<ABobPlayerState>(InstigatorPlayer->GetPlayerState()))
		{
			InstigatorState->ChangeCredits(1);
		}
	}
}

void ABobGameModeBase::RespawnPlayerElapsed(AController* Cont)
{
	if (ensure(Cont))
	{
		Cont->UnPossess();	
		RestartPlayer(Cont);
	}
}

void ABobGameModeBase::SpawnBotsTimerElapsed()
{
	if (!CVarSpawnBots.GetValueOnGameThread())
	{
		UE_LOG(LogTemp, Warning, TEXT("Disabled bot spawning through the console, ignoring bot spawn signal"));
		return;
	}

	int32 NOfAliveBots = 0;
	for (TActorIterator<ABobAICharacter> It(GetWorld()); It; ++It)
	{
		ABobAICharacter* Bot = *It;

		UBobAttributeComponent* AttComp = UBobAttributeComponent::GetAttributes(Bot);
		if (ensure(AttComp) && AttComp->IsAlive())
		{
			NOfAliveBots++;
		}
	}

	float MaxBotCount = 10.0f;
	if (DifficultyCurve)
	{
		MaxBotCount = DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}

	if (NOfAliveBots >= MaxBotCount)
	{
		return;
	}


	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnBotQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);

	if (ensure(QueryInstance))
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ABobGameModeBase::OnQueryCompleted);
	}
}

void ABobGameModeBase::OnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn Bot EQS Query Failed!"));
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

	if (Locations.IsValidIndex(0))
	{

		if (MonsterTable)
		{
			TArray<FMonsterInfoRow*> Rows; // Need to make an array of pointers to FMonsterInfoRow for GetAllRows

			MonsterTable->GetAllRows("", Rows); // First param is an identifier, only useful for logging if that is being done

			// Get a random entry from the array
			int32 RandomIndex = FMath::RandRange(0, Rows.Num() - 1);
			FMonsterInfoRow* SelectedRow = Rows[RandomIndex]; // Sets new variable SelectedRow (a pointer to the struct for MonsterTable) 



			UAssetManager* Manager = UAssetManager::GetIfValid();
			if (Manager)  // Lecture 27, Primary Assets and Async Loading 9:00
			{
				TArray<FName> Bundles;
				FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &ABobGameModeBase::OnMonsterLoaded, SelectedRow->MonsterId, Locations[0]);

				Manager->LoadPrimaryAsset(SelectedRow->MonsterId, Bundles, Delegate); // This sends the request to load the asset.
			}
			

		}

	}
}

void ABobGameModeBase::OnMonsterLoaded(FPrimaryAssetId LoadedId, FVector SpawnLocation)
{

	UAssetManager* Manager = UAssetManager::GetIfValid();
	if (Manager)  // Lecture 27, Primary Assets and Async Loading 15:00
	{
		UBobMinionDataAsset* MonsterData = Cast<UBobMinionDataAsset>(Manager->GetPrimaryAssetObject(LoadedId));
		if (MonsterData)
		{


			AActor* NewBot = GetWorld()->SpawnActor<AActor>(MonsterData->MonsterClass, SpawnLocation, FRotator::ZeroRotator); // Keeps reference to the new bot to apply status effects.
			//DrawDebugSphere(GetWorld(), Locations[0], 50.0f, 20, FColor::Blue, false, 60.0f); // This line draws a sphere where the query decides is a good spot to spawn a bot.


			if (NewBot)
			{
				// This log prints the system name for the bot and what type of enemy it is labeled as in the data blueprint.
				//LogOnScreen(this, FString::Printf(TEXT("Spawned enemy: %s (%s)"), *GetNameSafe(NewBot), *GetNameSafe(MonsterData)));

				// Grant special actions, buffs etc.
				UBobActionComponent* ActionComp = Cast<UBobActionComponent>(NewBot->GetComponentByClass(UBobActionComponent::StaticClass()));
				if (ActionComp)
				{
					for (TSubclassOf<UBobAction> ActionClass : MonsterData->Actions)
					{
						ActionComp->AddAction(NewBot, ActionClass); // This block of code iterates all of the actions in MonsterData and adds them to the action comp of the new bot actor.
					}
				}
			}


		}
	}



	
}


void ABobGameModeBase::KillAll()
{
	for (TActorIterator<ABobAICharacter> It(GetWorld()); It; ++It)
	{
		ABobAICharacter* Bot = *It;

		UBobAttributeComponent* AttComp = UBobAttributeComponent::GetAttributes(Bot);
		if (ensure(AttComp) && AttComp->IsAlive())
		{
			AttComp->Kill(this); // @fixme: link player instead of game mode
		}
	}
}


void ABobGameModeBase::WriteSaveGame()
{

	// Clear list before adding any actors so we don't get repeats
	CurrentSave->SavedActors.Empty();

	for (int32 i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		ABobPlayerState* PS = Cast<ABobPlayerState>(GameState->PlayerArray[i]); // GameState needs to be included manually, VA puts GameMode instead
		if (PS)
		{
			PS->SavePlayerState(CurrentSave);
			break; // @fixme: This is only for single player functionality, Lecture 23 Serializing Player Progression
		}
	}

	for (FActorIterator It(GetWorld()); It; ++It) 
	{
		AActor* Actor = *It;
		if (!Actor->Implements<UBobGameplayInterface>()) // This is meant to limit the amount of actors which are saved to only ones which implement this interface.
		{
			//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, Actor->GetName());
			continue;
		}

		//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, Actor->GetName());
		UE_LOG(LogTemp, Log, TEXT("Confirmed Save: %s"), *GetNameSafe(Actor));
		
		FActorSaveData ActorData;
		ActorData.ActorName = Actor->GetName();
		ActorData.ActorTransform = Actor->GetActorTransform();


		FMemoryWriter MemWriter(ActorData.ByteData); // Used for converting variables into bytes for storage.
		FObjectAndNameAsStringProxyArchive Ar(MemWriter, true); // Finds variables with the SaveGame identifier in UPROPERTY
		Ar.ArIsSaveGame = true; // Labels this as used for SaveGame functionality.

		Actor->Serialize(Ar); // Converts variables into a binary array and stores them.


		CurrentSave->SavedActors.Add(ActorData); // ***
	}



	UGameplayStatics::SaveGameToSlot(CurrentSave, SlotName, 0);
}

void ABobGameModeBase::ReadSaveGame()
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, "World Loaded!");
		CurrentSave = Cast<UBobSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
		if (CurrentSave == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load SaveGame Data."));
			return;
		}

		UE_LOG(LogTemp, Log, TEXT("Loaded SaveGame Data."));

		for (FActorIterator It(GetWorld()); It; ++It)
		{
			AActor* Actor = *It;
			UE_LOG(LogTemp, Log, TEXT("Pending Load: %s"), *GetNameSafe(Actor));
			if (!Actor->Implements<UBobGameplayInterface>()) // This is meant to limit the amount of actors which are loaded to only ones which implement this interface.
			{
				//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, Actor->GetName());
				UE_LOG(LogTemp, Log, TEXT("Ignored Load: %s"), *GetNameSafe(Actor));
				continue;
			}
			//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, Actor->GetName());
			UE_LOG(LogTemp, Log, TEXT("Confirmed Load: %s"), *GetNameSafe(Actor));

			for (FActorSaveData ActorData : CurrentSave->SavedActors)
			{
				if (ActorData.ActorName == Actor->GetName())
				{
					Actor->SetActorTransform(ActorData.ActorTransform); 


					FMemoryReader MemReader(ActorData.ByteData); // Used for converting bytes back into variables.
					FObjectAndNameAsStringProxyArchive Ar(MemReader, true); // Finds variables with the SaveGame identifier in UPROPERTY
					Ar.ArIsSaveGame = true; // Labels this as used for SaveGame functionality.

					Actor->Serialize(Ar); // Since Ar uses a memory reader, this function now converts the bytes back to variables.
					
					IBobGameplayInterface::Execute_OnActorLoaded(Actor); // Note: When accessing an interface, it uses the prefix I but when referencing it the prefix is U

					break;
				}
			}
		}
	}
	else
	{
		CurrentSave = Cast<UBobSaveGame>(UGameplayStatics::CreateSaveGameObject(UBobSaveGame::StaticClass()));

		UE_LOG(LogTemp, Log, TEXT("Created New SaveGame Data."));
	}

}



