// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BobGameplayInterface.h"
#include "BobSaveActor.generated.h"

class UStaticMeshComponent;
class UParticleSystemComponent;

UCLASS()
class ACTIONROUGELIKE_API ABobSaveActor : public AActor, public IBobGameplayInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABobSaveActor();

protected:
	// Called when the game starts or when spawned
	//virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* FireParticles;

	UPROPERTY(ReplicatedUsing = "OnRep_FireActivated", BlueprintReadOnly, SaveGame) // RepNotify
	bool bFireActivated;

	UFUNCTION()
	void OnRep_FireActivated();

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	void Interact_Implementation(APawn* InstigatorPawn);

	void OnActorLoaded_Implementation();
};
