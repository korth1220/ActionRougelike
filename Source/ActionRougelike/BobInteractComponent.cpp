// Fill out your copyright notice in the Description page of Project Settings.


#include "BobInteractComponent.h"
#include "BobGameplayInterface.h"
#include <DrawDebugHelpers.h>
#include "BobWorldUserWidget.h"

static TAutoConsoleVariable<bool> CVarDrawInteractSphere(TEXT("bob.DrawInteractSphere"), false, TEXT("Show where the interact line trace is actually going"), ECVF_Cheat); // Lecture 15: Console Variables

UBobInteractComponent::UBobInteractComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	TraceDistance = 600.0f;
	TraceRadius = 30.0f;

	CollisionChannel = ECC_WorldDynamic;
}


void UBobInteractComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	APawn* MyPawn = Cast<APawn>(GetOwner());
	if (MyPawn->IsLocallyControlled())
	{
		FindBestInteractable();
	}

}

void UBobInteractComponent::PrimaryInteract()
{
	ServerInteract(FocusedActor);
}

void UBobInteractComponent::FindBestInteractable()
{
	bool bDebugDraw = CVarDrawInteractSphere.GetValueOnGameThread();


	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(CollisionChannel); // Currently doesn't work with physics actors, just dynamic.

	AActor* MyOwner = GetOwner();

	FVector End;

	FVector EyeLocation;
	FRotator EyeRotation;

	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	End = EyeLocation + (EyeRotation.Vector() * TraceDistance);

	//bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(Hit, EyeLocation, End, ObjectQueryParams);

	TArray<FHitResult> Hits;
	FCollisionShape Shape;
	Shape.SetSphere(30.0f);

	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, Shape);

	FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;

	// Clears focus before trying to fill it
	FocusedActor = nullptr;


	for (FHitResult Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{
			if (HitActor->Implements<UBobGameplayInterface>())
			{
				FocusedActor = HitActor;

				if (bDebugDraw)
					DrawDebugSphere(GetWorld(), Hit.ImpactPoint, TraceRadius, 32, LineColor, false, 2.0f);
				break;
			}
		}
	}

	if (FocusedActor)
	{
		if (DefaultWidgetInstance == nullptr && ensure(DefaultWidgetClass))
		{
			DefaultWidgetInstance = CreateWidget<UBobWorldUserWidget>(GetWorld(), DefaultWidgetClass);
		}

		if (DefaultWidgetInstance)
		{
			DefaultWidgetInstance->AttachedActor = FocusedActor;

			if (!DefaultWidgetInstance->IsInViewport())
			{
				DefaultWidgetInstance->AddToViewport();
			}

		}

	}
	else
	{
		if (DefaultWidgetInstance)
		{
			DefaultWidgetInstance->RemoveFromParent();
		}
	}


	if (bDebugDraw)
		DrawDebugLine(GetWorld(), EyeLocation, End, LineColor, false, 2.0f, 0, 2.0f);
}

void UBobInteractComponent::ServerInteract_Implementation(AActor* INFocus)
{
	if (INFocus == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "No Focused Actor to Interact");
		return;
	}

	APawn* OwnerPawn = Cast<APawn>(GetOwner());

	IBobGameplayInterface::Execute_Interact(INFocus, OwnerPawn);
}
