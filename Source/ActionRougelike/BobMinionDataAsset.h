// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BobMinionDataAsset.generated.h"

class UBobAction;
/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API UBobMinionDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	TSubclassOf<AActor> MonsterClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	TArray<TSubclassOf<UBobAction>> Actions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	UTexture2D* Icon;


	FPrimaryAssetId GetPrimaryAssetId() const override 
	{
		return FPrimaryAssetId("Monsters", GetFName());
	} // Lecture 27, Primary Assets and Async Loading 6:00

};
