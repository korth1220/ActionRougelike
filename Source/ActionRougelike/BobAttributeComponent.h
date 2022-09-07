// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BobAttributeComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, AActor*, InstigatorActor, UBobAttributeComponent*, OwningComp, float, NewHealth, float, D);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnRageChanged, AActor*, InstigatorActor, UBobAttributeComponent*, OwningComp, float, NewRage, float, D);
// Applying multiple things onto the same event is covered in Assignment 6 review, not portrayed here.

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROUGELIKE_API UBobAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBobAttributeComponent();

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	static UBobAttributeComponent* GetAttributes(AActor* FromActor);

	UFUNCTION(BlueprintCallable, Category = "Attributes", meta = (DisplayName = "IsAlive")) // lecture 13 for statics and meta
	static bool IsActorAlive(AActor* Actor);

protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Replicated, Category = "Attributes")
	float Health;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Replicated, Category = "Attributes")
	float MaxHealth;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Replicated, Category = "Attributes")
	float Rage;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Replicated, Category = "Attributes")
	float MaxRage;

	UFUNCTION(NetMulticast, Reliable) // @fixme: Make this unreliable after we no longer use this event on the client for determining death
	void MulticastHealthChanged(AActor* InstigatorActor, float NewHealth, float delta);	

	UFUNCTION(NetMulticast, Unreliable) // @fixme: Make this unreliable after we no longer use this event on the client for determining death
	void MulticastRageChanged(AActor* InstigatorActor, float NewRage, float delta);

public:	

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyHeathChange(AActor* InstigatorActor, float delta);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyRageChange(AActor* InstigatorActor, float delta);

	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnRageChanged OnRageChanged;
		
	// -- Get START --

	UFUNCTION(BlueprintCallable)
	float GetHealth() const;

	UFUNCTION(BlueprintCallable)
	float GetHealthMax() const;

	UFUNCTION(BlueprintCallable)
	float GetRage() const;

	UFUNCTION(BlueprintCallable)
	float GetRageMax() const;

	// -- Get END --

	UFUNCTION(BlueprintCallable)
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable)
	void Kill(AActor* Killer);
};
