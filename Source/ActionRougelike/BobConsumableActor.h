// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BobGameplayInterface.h"
#include "BobConsumableActor.generated.h"

class USphereComponent;

UCLASS()
class ACTIONROUGELIKE_API ABobConsumableActor : public AActor, public IBobGameplayInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABobConsumableActor();

protected:
	// Called when the game starts or when spawned
	//virtual void BeginPlay() override;

	FTimerHandle TimerHandle_ConsumableCooldown;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* SphereComp;

	UFUNCTION(BlueprintCallable)
	void ConsumableVisibility(bool B);

	void CooldownElapsed();

	UFUNCTION()
	void OnRep_Use();

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

public:
	//void Interact_Implementation(APawn* InstigatorPawn) override; // override this in derived classes
	
	UFUNCTION(BlueprintCallable)
	void UseConsumable();

	UPROPERTY(EditAnywhere)
	float CooldownDuration;

	UPROPERTY(ReplicatedUsing = "OnRep_Use", BlueprintReadOnly, SaveGame) // RepNotify
	bool bUseable;

	FText GetInteractText_Implementation(APawn* InstigatorPawn);

};
